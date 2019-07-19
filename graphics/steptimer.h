#pragma once

#include <cmath>
#include <exception>
#include <stdint.h>

// fps = frames per second
// tpf = ticks per frame
// tps = ticks per second
// f   = clock frequency

namespace DX
{
	class StepTimer
	{
	public:

		StepTimer() noexcept(false) :
			f{ 0 }, t1{ 0 }, t2{ 0 }, fps{ 60 }, tps{ 0 }, tpf{ 0 }, fixed_step{ false }, delta{ 0 }, 
			current_fps{ 0 }, elapsed{ 0 }, elapsed_adjusted{ 0 }, c{ 0 }, n{ 0 }, age{ 0 }
		{
			LARGE_INTEGER f_, t1_;
			if (!QueryPerformanceFrequency(&f_))
				throw std::exception("QueryPerformanceFrequency");
			if (!QueryPerformanceCounter(&t1_))
				throw std::exception("QueryPerformanceCounter");
			f = f_.QuadPart;
			t1 = t1_.QuadPart;

			tpf = f / fps;
			tps = static_cast<uint64_t>(10e6);
			dlimit = f / 10;
			epsilon = tps * static_cast<uint64_t>(0.25 * 10e-4);
		}

		void Reset()
		{
			LARGE_INTEGER t1_;
			if (!QueryPerformanceCounter(&t1_))
				throw std::exception("QueryPerformanceCounter");
			t1 = t1_.QuadPart;
			n = 0;
			elapsed = 0;
			elapsed_adjusted = 0;
			current_fps = 0;
		}

		uint64_t Age() const { return age; }
		double AgeInSeconds() const { return double(age) / double(f); }
		uint64_t FrameDelta() const { return delta; }
		double FrameDeltaInSeconds() const { return double(delta) / double(f); }
		uint64_t FrameCount() const { return c; }
		uint64_t Frequency() const { return f; }

		template<typename TUpdate>
		void Tick(const TUpdate& update)
		{
			LARGE_INTEGER t2_;
			if (!QueryPerformanceCounter(&t2_))
				throw std::exception("QueryPerformanceCounter");
			t2 = t2_.QuadPart;

			delta = t2 - t1;
			elapsed += delta;
			t1 = t2;
			n++;

			if (!fixed_step)
			{
				update();
				c++;
			}
			else
			{
				if (std::abs(static_cast<int64_t>(delta - tpf)) < epsilon)
					delta = tpf;
				elapsed_adjusted += delta;
				if (elapsed_adjusted >= tpf)
				{
					uint64_t m = elapsed_adjusted / tpf;
					for (uint64_t i = 0; i < m; ++i)
						update();
					c += m;
					elapsed_adjusted %= f;
				}
			}

			if (elapsed >= f)
			{
				current_fps = n;
				n = 0;
				elapsed -= f;
			}

			age += delta;
		}

	private:
		uint64_t f, t1, t2, delta, dlimit, fps, current_fps, tps, tpf, elapsed, elapsed_adjusted;
		uint64_t c, n, age;
		int64_t epsilon;
		bool fixed_step;
	};
}