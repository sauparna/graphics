#include "progressreporter.h"
#include "parallel.h"
#include "stats.h"
#include <Windows.h>

namespace graphics
{
	static int TerminalWidth();

	ProgressReporter::ProgressReporter(int64_t totalWork, const std::string& title)
		: totalWork{ std::max((int64_t)1, totalWork) },
		  title{ title },
		  startTime{ std::chrono::system_clock::now() }
	{
		workDone = 0;
		exitThread = false;
		// Launch thread to periodically update progress bar
		if (!RayTracerOptions.quiet)
		{
			// We need to temporarily disable the profiler before launching the update thread
			// here, through the time the thread calls ProfilerWorkerThreadInit(). Otherwise,
			// there's a potential deadlock if the profiler interrup fires in the progress
			// reporter's thread and we try to access the thread-local ProfilerState variable
			// in the signal handler for the first time. (Which in turn calls malloc, which isn't
			// allowed in a signal handler.)
			SuspendProfiler();
			std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(2);
			updateThread = std::thread([this, barrier]()
				{
					ProfilerWorkerThreadInit();
					ProfilerState = 0;
					barrier->Wait();
					PrintBar();
				});
			//Wait for the thread to get past the ProfilerWorkerThreadInit() call
			barrier->Wait();
			ResumeProfiler();
		}
	}

	ProgressReporter::~ProgressReporter()
	{
		if (!RayTracerOptions.quiet)
		{
			workDone = totalWork;
			exitThread = true;
			updateThread.join();
			printf("\n");
		}
	}

	void ProgressReporter::PrintBar()
	{
		int barLength = TerminalWidth() - 28;
		int totalPlusses = std::max(2, barLength - (int)title.size());
		int plussesPrinted = 0;

		// Initialize progress string
		const int bufLen = title.size() + totalPlusses + 64;
		std::unique_ptr<char[]> buf(new char[bufLen]);
		snprintf(buf.get(), bufLen, "\r%s: [", title.c_str());
		char* curSpace = buf.get() + strlen(buf.get());
		char* s = curSpace;
		for (int i = 0; i < totalPlusses; ++i) *s++ = ' ';
		*s++ = ']';
		*s++ = ' ';
		*s++ = '\0';
		fputs(buf.get(), stdout);
		fflush(stdout);

		std::chrono::milliseconds sleepDuration(250);
		int iterCount = 0;
		while (!exitThread)
		{
			std::this_thread::sleep_for(sleepDuration);

			// Periodically increase sleepDuration to reduce update overhead.
			// Up to 0.5s after ~2.5s, up to 1s afters another ~30s and after
			// 15min, jump to 5s intervals.
			++iterCount;
			if (iterCount == 10)
				sleepDuration *= 2;
			else if (iterCount == 70)
				sleepDuration *= 2;
			else if (iterCount == 520)
				sleepDuration *= 5;

			Float percentDone = Float(workDone) / Float(totalWork);
			int plussesNeeded = std::round(totalPlusses * percentDone);
			while (plussesPrinted < plussesNeeded)
			{
				*curSpace++ = '+';
				++plussesPrinted;
			}
			fputs(buf.get(), stdout);

			// Update elapsed time and estimated time to completion
			Float seconds = ElapsedMS() / 1000.f;
			Float estRemaining = seconds / percentDone - seconds;
			if (percentDone == 1.f)
				printf(" (%.1fs)       ", seconds);
			else if (!std::isinf(estRemaining))
				printf(" (%.1fs|%.1fs)  ", seconds,
					std::max((Float)0., estRemaining));
			else
				printf(" (%.1fs|?s)  ", seconds);
			fflush(stdout);
		}
	}

	void ProgressReporter::Done() { workDone = totalWork; }

	static int TerminalWidth()
	{
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (h == INVALID_HANDLE_VALUE || !h)
		{
			fprintf(stderr, "GetStdHandle() call failed");
			return 80;
		}
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo = { 0 };
		GetConsoleScreenBufferInfo(h, &bufferInfo);
		return bufferInfo.dwSize.X;
	}
}
