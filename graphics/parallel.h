#pragma once

#include "graphics.h"
#include "geometry.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace graphics
{
	class AtomicFloat
	{
	public:
		explicit AtomicFloat(Float v = 0) { bits = FloatToBits(v); }
		operator Float() const { return BitsToFloat(bits); }
		Float operator=(Float v)
		{
			bits = FloatToBits(v);
			return v;
		}
		void Add(Float v)
		{
#ifdef GRAPHICS_FLOAT_AS_DOUBLE
			uint64_t oldBits = bits, newBits;
#else
			uint32_t oldBits = bits, newBits;
#endif
			do
			{
				newBits = FloatToBits(BitsToFloat(oldBits) + v);
			} while (!bits.compare_exchange_weak(oldBits, newBits));
		}

	private:
#ifdef GRAPHICS_FLOAT_AS_DOUBLE
		std::atomic<uint64_t> bits;
#else
		std::atomic<uint32_t> bits;
#endif
	};

	// Simple one-use barrier; ensures that multiple threads reach a particular point of execution before
	// proceeding further. Note: This should be heap-allocated and managed with a shared_ptr, where all
	// threads that use it are passed the shared_ptr. This ensures that memory for the Barrier won't be
	// freed until all threads have successfully cleared it.
	class Barrier
	{
	public:
		Barrier(int count) : count{ count } { assert(count > 0); }
		~Barrier() { assert(count == 0); }
		void Wait();

	private:
		std::mutex mutex;
		std::condition_variable cv;
		int count;
	};

	void ParallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize = 1);
	extern thread_local int ThreadIndex;
	void PrallelFor2D(std::function<void(Point2i)> func, const Point2i& count);
	int MaxThreadIndex();
	int NumSystemCores();
	
	void ParallelInit();
	void ParallelCleanup();
	void MergeWorkerThreadStats();

} // namespace graphics
