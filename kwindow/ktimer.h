#pragma once

#include "kwindow_pch.h"

class KTimer
{
public:
	KTimer()
	{
		reset();
	}

	void reset()
	{
		if (!QueryPerformanceFrequency(&f_))
			throw std::exception("QueryPerformanceFrequency");
		f = f_.QuadPart;
		get_time();
		get_time();
		overhead = std::abs(t2 - t1);
	}

	void get_time()
	{
		if (!QueryPerformanceCounter(&t_))
			throw std::exception("QueryPerformanceCounter");
		if (flag == 1)
			t1 = t_.QuadPart;
		else
			t2 = t_.QuadPart;
		flag *= -1;
	}

	double elapsed(int64_t units = 1E6)
	{
		return convert(std::abs(t2 - t1) - overhead, units);
	}

	double get_overhead(int64_t units = 1E6) { return convert(overhead, units); }

private:
	int64_t t1, t2, f, overhead;
	int flag{ 1 };
	LARGE_INTEGER t_, f_;

	double convert(int64_t ticks, int64_t units = 1E6)
	{
		int64_t d = ticks * units;
		return static_cast<double>(d) / static_cast<double>(f);
	}
};
