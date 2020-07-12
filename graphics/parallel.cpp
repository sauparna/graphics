#include "parallel.h"
#include "memory.h"
#include "stats.h"
#include <list>
#include <thread>
#include <condition_variable>

namespace graphics
{
	// Parallel local definitions
	static std::vector<std::thread> threads;
	static bool shutdownThreads = false;
	class ParallelForLoop;
	static ParallelForLoop* workList = nullptr;
	static std::mutex workListMutex;
	static std::condition_variable workListCondition;
	thread_local int ThreadIndex;

	// Bookkeeping variables to help with the implementation of MergeWorkerThreadStats().
	static std::atomic<bool> reportWorkerStats{ false };
	// Number of workers that still need to report their stats.
	static std::atomic<int> reporterCount;
	// After pushing the workers to report their stats, the main thread waits on this condition variable until
	// they've all done so.
	static std::condition_variable reportDoneCondition;
	static std::mutex reportDoneMutex;

	class ParallelForLoop
	{
	public:
		ParallelForLoop(std::function<void(int64_t)> func1D, int64_t maxIndex, int chunkSize, uint64_t profilerState)
			: func1D{ std::move(func1D) }, maxIndex{ maxIndex }, chunkSize{ chunkSize }, profilerState{ profilerState }
		{}
		ParallelForLoop(const std::function<void(Point2i)>& func2D, const Point2i& count, uint64_t profilerState)
			: func2D{ func2D }, maxIndex{ count.x * count.y }, chunkSize{ 1 }, profilerState{ profilerState }
		{
			nX = count.x;
		}

	public:
		std::function<void(int64_t)> func1D;
		std::function<void(Point2i)> func2D;
		const int64_t maxIndex;
		const int chunkSize;
		uint64_t profilerState;
		int64_t nextIndex = 0;
		int activeWorkers = 0;
		ParallelForLoop* next = nullptr;
		int nX = -1;

		bool Finished() const
		{
			return nextIndex >= maxIndex && activeWorkers == 0;
		}
	};

	void Barrier::Wait()
	{
		std::unique_lock<std::mutex> lock{ mutex };
		assert(count > 0);
		// If it's the last thread to reach the barrier; wake up all the other threads before exiting otherwise
		// give up the lock and wait to be notified as there are still threads that haven't reached.
		if (--count == 0)
			cv.notify_all();
		else
			cv.wait(lock, [this] {return count == 0; });
	}

	int MaxThreadIndex()
	{
		return RayTracerOptions.nThreads == 0 ? NumSystemCores() : RayTracerOptions.nThreads;
	}

	int NumSystemCores()
	{
		return std::max(1u, std::thread::hardware_concurrency());
	}

	static void workerThreadFunc(int tIndex, std::shared_ptr<Barrier> barrier)
	{
		// TODO: log this msg: "Started execution in worker thread " << tIndex;
		ThreadIndex = tIndex;

		// Give the profiler a chance to do per-thread initialization for the worker thread before the profiling
		// system actually stops running.
		ProfilerWorkerThreadInit();

		// The main thread sets up a barrier to make sure all workers have called ProfilerWorkerThreadInit() before
		// it continues (and actually starts the profiling system). Then release the reference to the Barrier so
		// that it's freed once all of the threads have gone past it.
		barrier->Wait();
		barrier.reset();

		std::unique_lock<std::mutex> lock{ workListMutex };
		while (!shutdownThreads)
		{
			// If stats-reporting is on, and all worker threads have merged their statts, wake up the main thread and
			// then sleep again. Otherwise, sleep until there are more tasks to run. Finally, if the first two conditions
			// don't hold, get work from workList and run loop iterations.
			if (reportWorkerStats)
			{
				ReportThreadStats();
				if (--reporterCount == 0)
					reportDoneCondition.notify_one();
				workListCondition.wait(lock);
			}
			else if (!workList)
				workListCondition.wait(lock);
			else
			{
				ParallelForLoop& loop = *workList;

				// Run a chunk of loop iteration for 'loop'

				// Find the set of loop iterations to run next
				int64_t indexStart = loop.nextIndex;
				int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);
				loop.activeWorkers++;

				// Run loop indices in [indexStart, indexEnd)
				lock.unlock();
				for (int64_t index = indexStart; index < indexEnd; ++index)
				{
					uint64_t oldState = ProfilerState;
					ProfilerState = loop.profilerState;
					if (loop.func1D)
						loop.func1D(index);
					else
					{
						assert(loop.func2D);
						loop.func2D(Point2i(index % loop.nX, index / loop.nX));
					}
					ProfilerState = oldState;
				}
				lock.lock();
				// Update loop to reflect completion of iterations
				loop.activeWorkers--;
				if (loop.Finished()) workListCondition.notify_all();
			}
		}
		// TODO: Log this msg: "Exiting worker thread " << tIndex;
	}

	void ParallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize)
	{
		assert(threads.size() > 0 || MaxThreadIndex() == 1);
		
		// Run iterations immediately if not using threads or if count is small
		if (threads.empty() || count < chunkSize)
		{
			for (int64_t i = 0; i < count; ++i) func(i);
			return;
		}

		// Create and enqueue ParallelForLoop for this loop
		ParallelForLoop loop(std::move(func), count, chunkSize, CurrentProfilerState());
		workListMutex.lock();
		loop.next = workList;
		workList = &loop;
		workListMutex.unlock();

		// Notify worker threads of work to be done
		std::unique_lock<std::mutex> lock{ workListMutex };
		workListCondition.notify_all();

		// Help out with parallel loop iterations in the current thread
		while (!loop.Finished())
		{
			// Run a chunk of loop iterations for 'loop'

			// Find the set of loop iterations to run next
			int64_t indexStart = loop.nextIndex;
			int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

			// Update 'loop' to reflect iterations this thread will run
			loop.nextIndex = indexEnd;
			if (loop.nextIndex == loop.maxIndex) workList = loop.next;
			loop.activeWorkers++;

			// Run loop indices in [indexStart, indexEnd)
			lock.unlock();
			for (int64_t index = indexStart; index < indexEnd; ++index)
			{
				uint64_t oldState = ProfilerState;
				ProfilerState = loop.profilerState;
				if (loop.func1D)
					loop.func1D(index);
				else
				{
					assert(loop.func2D);
					loop.func2D(Point2i(index % loop.nX, index / loop.nX));
				}
				ProfilerState = oldState;
			}
			lock.lock();
			// Update 'loop' to reflect completion of iterations
			loop.activeWorkers--;
		}
	}

	void PrallelFor2D(std::function<void(Point2i)> func, const Point2i& count)
	{
		assert(threads.size() > 0 || MaxThreadIndex() == 1);

		if(threads.empty() || count.x * count.y <= 1)
		{
			for (int y = 0; y < count.y; ++y)
				for (int x = 0; x < count.x; ++x) func(Point2i(x, y));
			return;
		}

		ParallelForLoop loop(std::move(func), count, CurrentProfilerState());

		{
			std::lock_guard<std::mutex> lock{ workListMutex };
			loop.next = workList;
			workList = &loop;
		}

		std::unique_lock<std::mutex> lock{ workListMutex };
		workListCondition.notify_all();

		// Help out with parallel loop iterations in the current thread
		while (!loop.Finished())
		{
			// Run a chunk of loop iterations for 'loop'

			// Find the set of loop iterations to run next
			int64_t indexStart = loop.nextIndex;
			int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

			// Update 'loop' to reflect iterations this thread will run
			loop.nextIndex = indexEnd;
			if (loop.nextIndex == loop.maxIndex) workList = loop.next;
			loop.activeWorkers++;

			// Run loop indices in [indexStart, indexEnd)
			lock.unlock();
			for (int64_t index = indexStart; index < indexEnd; ++index)
			{
				uint64_t oldState = ProfilerState;
				ProfilerState = loop.profilerState;
				if (loop.func1D)
					loop.func1D(index);
				else
				{
					assert(loop.func2D);
					loop.func2D(Point2i(index % loop.nX, index / loop.nX));
				}
				ProfilerState = oldState;
			}
			lock.lock();
			// Update 'loop' to reflect completion of iterations
			loop.activeWorkers--;
		}
	}

	void ParallelInit()
	{
		assert(threads.size() == 0);
		int nThreads = MaxThreadIndex();
		ThreadIndex = 0;

		// The barrier ensures all worker threads get past their call to ProfilerWorkerThreadInit() before returning
		// from this function. In turn, this ensures that the profiling system isn't started until after all worker
		// threads thave done that.
		std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(nThreads);

		// Launch one fewer worker thread than the total number since the main thread helps out too.
		for (int i = 0; i < nThreads - 1; ++i)
			threads.push_back(std::thread(workerThreadFunc, i + 1, barrier));

		barrier->Wait();
	}

	void ParallelCleanup()
	{
		if (threads.empty()) return;

		{
			std::lock_guard<std::mutex> lock{ workListMutex };
			shutdownThreads = true;
			workListCondition.notify_all();
		}

		for (std::thread& thread : threads) thread.join();
		threads.erase(threads.begin(), threads.end());
		shutdownThreads = false;
	}

	void MergeWorkerThreadStats()
	{
		std::unique_lock<std::mutex> lock{ workListMutex };
		std::unique_lock<std::mutex> doneLock{ reportDoneMutex };
		
		// Set up state so that the worker threads will know that we would like them to report their
		// thread-specific stats when they wake up.
		reportWorkerStats = true;
		reporterCount = threads.size();

		// Wake up the worker threads.
		workListCondition.notify_all();

		// Wait for all of them to merge their stats.
		reportDoneCondition.wait(lock, []() { return reporterCount == 0; });

		reportWorkerStats = false;
	}

} // namespace graphics