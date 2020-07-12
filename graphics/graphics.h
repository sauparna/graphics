#define NOMINMAX

#pragma once

// Global include files

#include <cassert>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

#pragma warning(disable : 4305)  // double constant assigned to float
#pragma warning(disable : 4244)  // int -> float conversion
#pragma warning(disable : 4843)  // double -> float conversion
#pragma warning(disable : 4267)  // size_t -> int
#pragma warning(disable : 4838)  // another double -> int

// Global macros
#define ALLOCA(TYPE, COUNT) (TYPE*) alloca((COUNT) * sizeof(TYPE))

namespace graphics
{
	// Global forward declarations
	class Scene;
	class Integrator;
	class SamplerIntegrator;
	template <typename T>
	class Vector2;
	template <typename T>
	class Vector3;
	template <typename T>
	class Point2;
	template <typename T>
	class Point3;
	template <typename T>
	class Normal3;
	class Ray;
	class RayDifferential;
	template <typename T>
	class Bounds2;
	template <typename T>
	class Bounds3;
	class Transform;
	class Interactions;
	class SurfaceInteraction;
	class Shape;
	class Primitive;
	class GeometricPrimitive;
	class TransformedPrimitive;
	template <int nSpectrumSamples>
	class CoefficientSpectrum;
	class RGBSpectrum;
	class SampledSpectrum;
#ifdef GRAPHCIS_SAMPLED_SPECTRUM
	typedef SampledSpectrum Spectrum;
#else
	typedef RGBSpectrum Spectrum;
#endif
	class Camera;
	class CameraSample;
	class ProjectiveCamera;
	class Sampler;
	class Filter;
	class Film;
	class FilmTile;
	class BxDF;
	class BRDF;
	class BSDF;
	class BTDF;
	class Material;
	template <typename T>
	class Texture;
	class Medium;
	class MediumInteraction;
	class MediumInterface;
	class BSSRDF;
	class SeparableBSSRDF;
	class TabulatedBSSRDF;
	class BSSRDFTable;
	class Light;
	class VisibilityTester;
	class AreaLight;
	class Distribution1D;
	class Distribution2D;
	class RNG;
	class ProgressReporter;
	class MemoryArena;
	template <typename T, int logBlockSize = 2>
	class BlockedArray;
	struct Matrix4x4;
	class ParamSet;
	template <typename T>
	struct ParamSetItem;
	class TextureParams;

#ifdef GRAPHICS_FLOAT_AS_DOUBLE
	typedef double Float;
#else
	typedef float Float;
#endif // GRAPHICS_FLOAT_AS_DOUBLE

	struct Options
	{
		Options()
		{
			cropWindow[0][0] = 0;
			cropWindow[0][1] = 1;
			cropWindow[1][0] = 0;
			cropWindow[1][1] = 1;
		}
		int nThreads = 0;
		bool quickRender = false;
		bool quiet = false;
		bool cat = false, toPly = false;
		std::string imageFile;
		// x0, x1, y0, y1
		Float cropWindow[2][2];
	};

	extern Options RayTracerOptions;
	

	// Global constants

#define MaxFloat std::numeric_limits<Float>::max()
#define Inf std::numeric_limits<Float>::infinity()
#define MachineEpsilon (std::numeric_limits<Float>::epsilon() * 0.5)
#define alloca _alloca

	static constexpr Float ShadowEpsilon = 0.0001f;
	static constexpr Float Pi = 3.14159265358979323846;
	static constexpr Float InvPi = 0.31830988618379067154;
	static constexpr Float Inv2Pi = 0.15915494309189533577;
	static constexpr Float Inv4Pi = 0.07957747154594766788;
	static constexpr Float PiOver2 = 1.57079632679489661923;
	static constexpr Float PiOver4 = 0.78539816339744830961;
	static constexpr Float Sqrt2 = 1.41421356237309504880;


	// Global inline functions

	inline uint32_t FloatToBits(float f)
	{
		uint32_t ui;
		memcpy(&ui, &f, sizeof(float));
		return ui;
	}
	inline float BitsToFloat(uint32_t ui)
	{
		float f;
		memcpy(&f, &ui, sizeof(uint32_t));
		return f;
	}
	inline uint64_t FloatToBits(double f)
	{
		uint64_t ui;
		memcpy(&ui, &f, sizeof(double));
		return ui;
	}
	inline float BitsToFloat(uint64_t ui)
	{
		float f;
		memcpy(&f, &ui, sizeof(uint64_t));
		return f;
	}
	inline Float gamma(int n)
	{
		return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
	}
} // namespace graphics