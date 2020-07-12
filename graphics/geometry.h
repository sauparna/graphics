#pragma once

#include "graphics.h"
#include "stringprint.h"
#include <iterator>

namespace graphics {

	// std::isnan() is not a template.
	template <typename T> inline bool isNaN(const T x) { return std::isnan(x); }
	template <> inline bool isNaN(const int x) { return false; }

	// Point Declarations

	template <typename T>
	class Point2
	{
	public:
		explicit Point2(const Point3<T>& p) : x{ p.x }, y{ p.y } { assert(!HasNaNs()); }
		Point2() { x = y = 0; }
		Point2(T xx, T yy) : x{ xx }, y{ yy } { assert(!HasNaNs()); }
		bool HasNaNs() const { return isNaN(x) || isNaN(y); }
		template <typename U>
		explicit Point2(const Point2<U>& p)
		{
			x = (T)p.x;
			y = (T)p.y;
			assert(!HasNaNs());
		}
		template<typename U>
		explicit Point2(const Vector2<U>& v)
		{
			x = (T)v.x;
			y = (T)v.y;
			assert(!HasNaNs());
		}
		template <typename U>
		explicit operator Vector2<U>() const
		{
			return Vector2<U>{x, y};
		}
		T operator[](int i) const
		{
			assert(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}
		T& operator[](int i)
		{
			assert(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}
		bool operator==(const Point2<T>& p) const { return x == p.x && y == p.y; }
		bool operator!=(const Point2<T>& p) const { return x != p.x || y != p.y; }
		Point2<T> operator+(const Vector2<T>& v) const
		{
			assert(!v.HasNaNs());
			return Point2<T>{x + v.x, y + v.y};
		}
		Point2<T>& operator+=(const Vector2<T>& v)
		{
			assert(!HasNaNs());
			x += v.x;
			y += v.y;
			return *this;
		}
		Point2<T> operator+(const Point2<T>& p) const
		{
			assert(!p.HasNaNs());
			return Point2<T>{x + p.x, y + p.y};
		}
		Point2<T>& operator+=(const Point2<T>& p)
		{
			assert(!HasNaNs());
			x += p.x;
			y += p.y;
			return *this;
		}
		Vector2<T> operator-(const Point2<T>& p) const
		{
			assert(!p.HasNaNs());
			return Vector2<T>{x - p.x, y - p.y};
		}
		Point2<T> operator-(const Vector2<T>& v) const
		{
			assert(!v.HasNaNs());
			return Point2<T>{x - v.x, y - v.y};
		}
		Point2<T>& operator-=(const Vector2<T>& v)
		{
			assert(!v.HasNaNs());
			x -= v.x;
			y -= v.y;
			return *this;
		}
		Point2<T> operator-() const { return Point2<T>{-x, -y}; }
		Point2<T> operator*(T f) const
		{
			return Point2<T>{x * f, y * f};
		}
		Point2<T>& operator*=(T f)
		{
			assert(!isNaN(f));
			x *= f;
			y *= f;
			return *this;
		}
		Point2<T> operator/(T f)
		{
			assert(f != 0);
			T inv = (T)1 / f;
			return Point2<T>{x * inv, y * inv};
		}
		Point2<T>& operator/=(T f)
		{
			assert(f != 0);
			T inv = (T)1 / f;
			x *= inv;
			y *= inv;
			return *this;
		}

		// Point2 public datat
		T x, y;
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Point2<T>& v)
	{
		os << "[ " << v.x << ", " << v.y << " ]";
		return os;
	}
	template <>
	inline std::ostream& operator<<(std::ostream& os, const Point2<Float>& v)
	{
		os << StringPrintf("[ %f, %f ]", v.x, v.y);
		return os;
	}

	template <typename T>
	class Point3
	{
	public:
		Point3() { x = y = z = 0; }
		Point3(T x, T y, T z) : x{ x }, y{ y }, z{ z } { assert(!HasNaNs()); }
		bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
		template <typename U>
		explicit Point3(const Point3<U>& p)
			: x{ (T)p.x }, y{ (T)p.y }, z{ (T)p.z }
		{
			assert(!HasNaNs());
		}
		template <typename U>
		explicit operator Vector3<U>() const
		{
			return Vector3<U>(x, y, z);
		}
		T operator[](int i) const
		{
			assert(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		T& operator[](int i)
		{
			assert(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		Point3<T> operator+(const Point3<T>& p) const
		{
			assert(!p.HasNaNs());
			return Point3<T>(x + p.x, y + p.y, z + p.z);
		}
		Point3<T> operator+(const Vector3<T>& v) const
		{
			assert(!v.HasNaNs());
			return Point3<T>(x + v.x, y + v.y, z + v.z);
		}
		Point3<T>& operator+=(const Point3<T>& p)
		{
			assert(!p.HasNaNs());
			x += p.x;
			y += p.y;
			z += p.z;
			return *this;
		}
		Point3<T>& operator+=(const Vector3<T>& v)
		{
			assert(!v.HasNaNs());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		Vector3<T> operator-(const Point3<T>& p) const
		{
			assert(!p.HasNaNs());
			return Vector3<T>(x - p.x, y - p.y, z - p.z);
		}
		Point3<T> operator-(const Vector3<T>& v) const
		{
			assert(!v.HasNaNs());
			return Point3<T>(x - v.x, y - v.y, z - v.z);
		}
		Point3<T>& operator-=(const Vector3<T>& v)
		{
			assert(!v.HasNaNs());
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		Point3<T> operator*(T f) const
		{
			return Point3<T>(x * f, y * f, z * f);
		}
		Point3<T>& operator*=(T f)
		{
			assert(!isNaN(f));
			x *= f;
			y *= f;
			z *= f;
			return *this;
		}
		Point3<T> operator/(T f)
		{
			assert(f != 0);
			T inv = (T)1 / f;
			return Point3<T>(x * inv, y * inv, z * inv);
		}
		Point3<T>& operator/=(T f)
		{
			assert(f != 0);
			T inv = (T)1 / f;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}

		// Point Public Data
		T x, y, z;
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Point3<T>& v)
	{
		os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
		return os;
	}
	template <>
	inline std::ostream& operator<<(std::ostream& os, const Point3<Float>& v)
	{
		os << StringPrintf("[ %f, %f, %f ]", v.x, v.y, v.z);
		return os;
	}

	typedef Point2<Float> Point2f;
	typedef Point2<int> Point2i;
	typedef Point3<Float> Point3f;
	typedef Point3<int> Point3i;

	// Vector declarations

	template <typename T>
	class Vector2
	{
	public:
		Vector2() { x = y = 0; }
		Vector2(T x, T y) : x{ x }, y{ y } { assert(!HasNaNs()); }
		bool HasNaNs() const { return isNaN(x) || isNaN(y); }
		explicit Vector2(const Point2<T>& p);
		explicit Vector2(const Point3<T>& p);
		T operator[](int i) const
		{
			assert(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}
		T& operator[](int i)
		{
			assert(i >= 0 && i <= 1);
			if (i == 0) return x;
			return y;
		}
		Vector2<T> operator+(const Vector2<T>& v) const
		{
			assert(!v.HasNaNs());
			return Vector2(x + v.x, y + v.y);
		}
		Vector2<T>& operator+=(const Vector2<T>& v)
		{
			assert(!v.HasNaNs());
			x += v.x;
			y += v.y;
			return *this;
		}
		Vector2<T> operator-(const Vector2<T>& v) const
		{
			assert(!v.HasNaNs());
			return Vector2(x - v.x, y - v.y);
		}
		Vector2<T>& operator-=(const Vector2<T>& v)
		{
			assert(!v.HasNaNs());
			x -= v.x;
			y -= v.y;
			return *this;
		}
		Vector2<T> operator-() const { return Vector2<T>{-x, -y}; }
		bool operator==(const Vector2<T>& v) const { return x == v.x && y == v.y; }
		bool operator!=(const Vector2<T>& v) const { return x != v.x || y != v.y; }
		Vector2<T> operator*(T s) const
		{
			return Vector2<T>(x * s, y * s);
		}
		Vector2<T>& operator*=(T s)
		{
			assert(!isNaN(s));
			x *= s;
			y *= s;
			return *this;
		}
		Vector2<T> operator/(T s) const
		{
			assert(s != 0);
			T inv = (T)1 / s;
			return Vector2<T>{x * inv, y * inv};
		}
		Vector2<T>& operator/=(T s)
		{
			assert(s != 0);
			T inv = (T)1 / s;
			x *= inv;
			y *= inv;
			return *this;
		}
		Float LengthSquared() const { return x * x + y * y; }
		Float Length() const { return std::sqrt(LengthSquared()); }

		// Vector2 public data
		T x, y;
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
	{
		os << "[ " << v.x << ", " << v.y << " ]";
		return os;
	}
	template <>
	inline std::ostream& operator<<(std::ostream& os, const Vector2<Float>& v)
	{
		os << StringPrintf("[ %f, %f ]", v.x, v.y);
		return os;
	}

	template <typename T>
	class Vector3
	{
	public:
		Vector3() { x = y = z = 0; };
		Vector3(T x, T y, T z) : x{ x }, y{ y }, z{ z } { assert(!HasNaNs()); }
		bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
		Float LengthSquared() const { return x * x + y * y + z * z; }
		Float Length() const { return std::sqrt(LengthSquared()); }
		T operator[](int i) const
		{
			assert(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		T& operator[](int i)
		{
			assert(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		Vector3<T> operator+(const Vector3<T>& v) const
		{
			return Vector3(x + v.x, y + v.y, z + v.z);
		}
		Vector3<T>& operator+=(const Vector3<T>& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		Vector3<T> operator-(const Vector3<T>& v) const
		{
			return Vector3(x - v.x, y - v.y, z - v.z);
		}
		Vector3<T>& operator-=(const Vector3<T>& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		Vector3<T> operator-() const { return Vector3<T>{-x, -y, -z}; }
		Vector3<T> operator*(T s) const
		{
			return Vector3<T>{x * s, y * s, z * s};
		}
		Vector3<T>& operator*=(T s)
		{
			assert(!isNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		Vector3<T> operator/(T s) const
		{
			assert(s != 0);
			T inv = (T)1 / s;
			return Vector3<T>{x * inv, y * inv, z * inv};
		}
		Vector3<T>& operator/=(T s)
		{
			assert(s != 0);
			T inv = (T)1 / s;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}

		// Vector3 public data
		T x, y, z;
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Vector3<T>& v)
	{
		os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
		return os;
	}
	template <>
	inline std::ostream& operator<<(std::ostream& os, const Vector3<Float>& v)
	{
		os << StringPrintf("[ %f, %f, %f ]", v.x, v.y, v.z);
		return os;
	}

	typedef Vector2<Float> Vector2f;
	typedef Vector2<int> Vector2i;
	typedef Vector3<Float> Vector3f;
	typedef Vector3<int> Vector3i;

	// Normal declarations

	template <typename T>
	class Normal3
	{
	public:
		Normal3() { x = y = z = 0; }
		Normal3(T x, T y, T z) : x{ x }, y{ y }, z{ z } { assert(!HasNaNs()); }
		bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
		explicit Normal3<T>(const Vector3<T>& v) : x{ v.x }, y{ v.y }, z{ v.z }
		{
			assert(!v.HasNaNs());
		}
		T operator[](int i) const
		{
			assert(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		T& operator[](int i)
		{
			assert(i >= 0 && i <= 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}
		bool operator==(const Normal3<T>& n) const
		{
			return x == n.x && y == n.y && z == n.z;
		}
		bool operator!=(const Normal3<T>& n) const
		{
			return x != n.x || y != n.y || z != n.z;
		}
		Normal3<T> operator+(const Normal3<T>& n) const
		{
			assert(!n.HasNaNs());
			return Normal3<T>{ x + n.x, y + n.y, z + n.z };
		}
		Normal3<T>& operator+=(const Normal3<T>& n)
		{
			assert(!n.HasNaNs());
			x += n.x;
			y += n.y;
			z += n.z;
			return *this;
		}
		Normal3<T> operator-(const Normal3<T>& n) const
		{
			assert(!n.HasNaNs());
			return Normal3<T>{x - n.x, y - n.y, z - n.z};
		}
		Normal3<T>& operator-=(const Normal3<T>& n)
		{
			assert(!n.HasNaNs());
			x -= n.x;
			y -= n.y;
			z -= n.z;
			return *this;
		}
		Normal3<T> operator-() const
		{
			return Normal3<T>{-x, -y, -z};
		}
		template <typename U>
		Normal3<T> operator*(U s) const
		{
			return Normal3<T>{x * s, y * s, z * s};
		}
		template <typename U>
		Normal3<T>& operator*=(U s)
		{
			assert(!isNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		Normal3<T> operator/(T s) const
		{
			assert(s != 0);
			T inv = (T)1 / s;
			return Normal3<T>{x * inv, y * inv, z * inv};
		}
		template <typename U>
		Normal3<T>& operator/=(U s)
		{
			assert(s != 0);
			Float inv = (Float)1 / s;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}
		Float LengthSquared() const { return x * x + y * y + z * z; }
		Float Length() const { return std::sqrt(LengthSquared()); }

		// Norma3 public data
		T x, y, z;
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const Normal3<T>& n)
	{
		os << "[ " << n.x << ", " << n.y << ", " << n.z << " ]";
		return os;
	}
	template <>
	inline std::ostream& operator<<(std::ostream& os, const Normal3<Float>& n)
	{
		os << StringPrintf("[ %f %f %f ]", n.x, n.y, n.z);
		return os;
	}

	typedef Normal3<Float> Normal3f;

	// Ray declarations

	class Ray
	{
	public:
		Ray() : tMax(Inf), time(0.f), medium(nullptr) {}
		Ray(const Point3f& o, const Vector3f& d, Float tMax = Inf, Float time = 0.f, const Medium* medium = nullptr)
			: o{ o }, d{ d }, tMax{ tMax }, time{ time }, medium{ medium } {}
		Point3f operator()(Float t) const { return o + d * t; }
		bool HasNaNs() const { return (o.HasNaNs() || d.HasNaNs() || isNaN(tMax)); }
		friend std::ostream& operator<<(std::ostream& os, const Ray& r)
		{
			os << "[o=" << r.o << ", d=" << r.d << ", tMax=" << r.tMax << ", time=" << r.time << "]";
			return os;
		}

		// Ray public data
		Point3f o;
		Vector3f d;
		mutable Float tMax;
		Float time;
		const Medium* medium;
	};

	class RayDifferential : public Ray
	{
	public:
		RayDifferential() { hasDifferentials = false; }
		RayDifferential(const Point3f& o, const Vector3f& d, Float tMax = Inf, Float time = 0.f, const Medium* medium = nullptr)
			: Ray{ o, d, tMax, time, medium }
		{
			hasDifferentials = false;
		}
		RayDifferential(const Ray& ray) : Ray{ ray } { hasDifferentials = false; }
		bool HasNaNs() const
		{
			return Ray::HasNaNs() || (hasDifferentials && 
				(rxOrigin.HasNaNs() || ryOrigin.HasNaNs() || rxDirection.HasNaNs() || ryDirection.HasNaNs()));
		}
		void ScaleDifferentials(Float s)
		{
			rxOrigin = o + (rxOrigin - o) * s;
			ryOrigin = o + (ryOrigin - o) * s;
			rxDirection = d + (rxDirection - d) * s;
			ryDirection = d + (ryDirection - d) * s;
		}
		friend std::ostream& operator<<(std::ostream& os, const RayDifferential& r)
		{
			os << "[ " << (Ray&)r << " has differentials: " << (r.hasDifferentials ? "true" : "false") <<
				", xo = " << r.rxOrigin << ", xd = " << r.rxDirection <<
				", yo = " << r.ryOrigin << ", yd = " << r.ryDirection;
			return os;
		}

		// RayDifferential public data
		bool hasDifferentials;
		Point3f rxOrigin, ryOrigin;
		Vector3f rxDirection, ryDirection;
	};

	// Bounds declarations
	template <typename T>
	class Bounds2
	{
	public:
		// The default values violate the invarian pMin.x <= pMax.x.
		Bounds2()
		{
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			pMin = Point2<T>(maxNum, maxNum);
			pMax = Point2<T>(minNum, minNum);
		}
		explicit Bounds2(const Point2<T>& p) : pMin{ p }, pMax{ p } {}
		Bounds2(const Point2<T>& p1, Point2<T>& p2)
		{
			pMin = Point2<T>{ std::min(p1.x, p2.x), std::min(p1.y, p2.y) };
			pMax = Point2<T>{ std::max(p1.x, p2.x), std::max(p1.y, p2.y) };
		}
		template <typename U>
		explicit operator Bounds2<U>() const
		{
			return Bounds2<U>{(Point2<U>)pMin, (Point2<U>)pMax};
		}

		Vector2<T> Diagonal() const { return pMax - pMin; }
		T Area() const
		{
			Vector2<T> d = pMax - pMin;
			return (d.x * d.y);
		}
		int MaximumExtent() const
		{
			Vector2<T> diag = Diagonal();
			if (diag.x > diag.y)
				return 0;
			return 1;
		}
		inline const Point2<T>& operator[](int i) const
		{
			assert(i == 0 || i == 1);
			return (i == 0) ? pMin : pMax;
		}
		inline Point2<T>& operator[](int i)
		{
			assert(i == 0 || i == 1);
			return (i == 0) ? pMin : pMax;
		}
		bool operator==(const Bounds2<T>& b) const
		{
			return b.pMin == pMin && b.pMax == pMax;
		}
		bool operator!=(const Bounds2<T>& b) const
		{
			return b.Min != pMin || b.pMax != pMax;
		}
		Point2<T> Lerp(const Point2f& t) const
		{
			return Point2<T>{Lerp(t.x, pMin.x, pMax.x), Lerp(t.y, pMin.y, pMax.y)};
		}
		Vector2<T> Offset(const Point2<T>& p) const
		{
			Vector2<T> o = p - pMin;
			if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
			if (pMax.y > pMin.y) o.y /= pMax.x - pMin.y;
			return o;
		}
		void BoundingSphere(Point2<T>* c, Float* rad) const
		{
			*c = (pMin + pMax) / 2;
			*rad = Inside(*c, *this) ? Distance(*c, pMax) : 0;
		}
		friend std::ostream& operator<<(std::ostream& os, const Bounds2<T>& b)
		{
			os << "[ " << b.pMin << b.pMin << " - " << b.pMax << " ]";
			return os;
		}

		// Bounds2 public data
		Point2<T> pMin, pMax;
	};

	template <typename T>
	class Bounds3
	{
	public:
		// The default values violate the invarian pMin.x <= pMax.x.
		Bounds3()
		{
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			pMin = Point3<T>(maxNum, maxNum, maxNum);
			pMax = Point3<T>(minNum, minNum, minNum);
		}
		explicit Bounds3(const Point3<T>& p) : pMin{ p }, pMax{ p } {}
		Bounds3(const Point3<T>& p1, const Point3<T>& p2)
			: pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)),
			  pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)) {}
		inline const Point3<T>& operator[](int i) const
		{
			assert(i == 0 || i == 1);
			return (i == 0) ? pMin : pMax;
		}
		inline Point3<T>& operator[](int i)
		{
			assert(i == 0 || i == 1);
			return (i == 0) ? pMin : pMax;
		}
		bool operator==(const Bounds3<T>& b) const
		{
			return b.pMin == pMin && b.pMax == pMax;
		}
		bool operator!=(const Bounds3<T>& b) const
		{
			return b.pMin != pMin || b.pMax != pMax;
		}
		Point3<T> Corner(int corner) const
		{
			assert(corner >= 0 && corner < 8);
			return Point3<T>(
				(*this)[(corner & 1)].x,
				(*this)[(corner & 2) ? 1 : 0].y,
				(*this)[(corner & 4) ? 1 : 0].z);
		}
		Vector3<T> Diagonal() const { return pMax - pMin; }
		T SurfaceArea() const
		{
			Vector3<T> d = Diagonal();
			return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
		}
		T Volume() const
		{
			Vector3<T> d = Diagonal();
			return d.x * d.y * d.z;
		}
		int MaximumExtent() const
		{
			Vector3<T> d = Diagonal();
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}
		Point3<T> Lerp(const Point3f& t) const
		{
			return Point3<T>{
				Lerp(t.x, pMin.x, pMax.x),
				Lerp(t.y, pMin.y, pMax.y),
				Lerp(t.z, pMin.z, pMax.z)
			};	
		}
		Vector3<T> Offset(const Point3<T>& p) const
		{
			Vector3<T> o = p - pMin;
			if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
			if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
			if (pMax.z > pMin.z) o.z /= pMax.z - pMin.z;
			return o;
		}
		void BoundingSphere(Point3<T>* center, Float* radius) const
		{
			*center = (pMin + pMax) / 2;
			*radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
		}
		template <typename U>
		explicit operator Bounds3<U>() const
		{
			return Bounds3<U>((Point3<U>)pMin, (Point3<U>)pMax);
		}
		bool IntersectP(const Ray& ray, Float* hitt0 = nullptr, Float* hitt1 = nullptr) const;
		inline bool IntersectP(const Ray& ray, const Vector3f& invDir, const int dirIsNeg[3]) const;
		friend std::ostream& operator<<(std::ostream& os, const Bounds3<T>& b)
		{
			os << "[ " << b.pMin << " - " << b.pMax << " ]";
			return os;
		}

		// Bounds3 public data
		Point3<T> pMin, pMax;
	};

	typedef Bounds2<Float> Bounds2f;
	typedef Bounds2<int> Bounds2i;
	typedef Bounds3<Float> Bounds3f;
	typedef Bounds3<int> Bounds3i;

	class Bounds2iIterator : public std::forward_iterator_tag
	{
	public:
		Bounds2iIterator(const Bounds2i& b, const Point2i& pt) : p{ pt }, bounds{ &b } {}
		Bounds2iIterator operator++()
		{
			advance();
			return *this;
		}
		Bounds2iIterator operator++(int)
		{
			Bounds2iIterator old = *this;
			advance();
			return old;
		}
		bool operator==(const Bounds2iIterator& bi) const
		{
			return p == bi.p && bounds == bi.bounds;
		}
		bool operator!=(const Bounds2iIterator& bi) const
		{
			return p != bi.p || bounds != bi.bounds;
		}
		Point2i operator*() const { return p; }

	private:
		void advance()
		{
			++p.x;
			if (p.x == bounds->pMax.x)
			{
				p.x = bounds->pMin.x;
				++p.y;
			}
		}
		Point2i p;
		const Bounds2i* bounds;
	};

	// Geometry inline functions
	// TODO: there may be glaring bugs in here!

	// Dot products

	template <typename T>
	inline T Dot(const Vector2<T>& v1, const Vector2<T>& v2)
	{
		assert(!v1.HasNaNs() && !v2.HasNaNs());
		return v1.x * v2.x + v1.y * v2.y;
	}
	template <typename T>
	inline T Dot(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		assert(!v1.HasNaNs() && !v2.HasNaNs());
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}
	template <typename T>
	inline T Dot(const Normal3<T>& n1, const Normal3<T>& n2)
	{
		assert(!n1.HasNaNs() && !n2.HasNaNs());
		return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
	}
	template <typename T>
	inline T Dot(const Normal3<T>& n, const Vector3<T>& v)
	{
		assert(!n.HasNaNs() && !v.HasNaNs());
		return n.x * v.x + n.y * v.y + n.z * v.z;
	}
	template <typename T>
	inline T Dot(const Vector3<T>& v, const Normal3<T>& n)
	{
		assert(!v.HasNaNs() && !n.HasNaNs());
		return v.x * n.x + v.y * n.y + v.z * n.z;
	}

	template <typename T>
	inline T AbsDot(const Vector2<T>& v1, const Vector2<T>& v2)
	{
		assert(!v1.HasNaNs() && !v2.HasNaNs());
		return std::abs(v1.x * v2.x + v1.y * v2.y);
	}
	template <typename T>
	inline T AbsDot(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		assert(!v1.HasNaNs() && !v2.HasNaNs());
		return std::abs(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}
	template <typename T>
	inline T AbsDot(const Normal3<T>& n, const Vector3<T>& v)
	{
		assert(!n.HasNaNs() && !v.HasNaNs());
		return std::abs(n.x * v.x + n.y * v.y + n.z * v.z);
	}
	template <typename T>
	inline T AbsDot(const Vector3<T>& v, const Normal3<T>& n)
	{
		assert(!v.HasNaNs() && !n.HasNaNs());
		return std::abs(v.x * n.x + v.y * n.y + v.z * n.z);
	}
	template <typename T>
	inline T AbsDot(const Normal3<T>& n1, const Normal3<T>& n2)
	{
		assert(!n1.HasNaNs() && !n2.HasNaNs());
		return std::abs(n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
	}

	// Cross product

	template <typename T>
	inline Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		// Convert to double-precision before the subtractoins to protect against error from catastrophic cancellation.
		double x1 = v1.x, y1 = v1.y, z1 = v1.z;
		double x2 = v2.x, y2 = v2.y, z2 = v2.z;
		return Vector3<T>(
			y1 * z2 - y2 * z1,
			x2 * z1 - x1 * z2,
			x1 * y2 - x2 * y1
			);
	}

	// Absolute values

	template <typename T>
	Vector2<T> Abs(const Vector2<T>& v)
	{
		return Vector2<T>(std::abs(v.x), std::abs(v.y));
	}
	template <typename T>
	Vector3<T> Abs(const Vector3<T>& v)
	{
		return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}
	template <typename T>
	Point2<T> Abs(const Point2<T>& p)
	{
		return Point2<T>(std::abs(p.x), std::abs(p.y));
	}
	template <typename T>
	Point3<T> Abs(const Point3<T>& p)
	{
		return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
	}
	template <typename T>
	Normal3<T> Abs(const Normal3<T>& n)
	{
		return Normal3<T>(std::abs(n.x), std::abs(n.y), std::abs(n.z));
	}

	// Min/Max

	template <typename T>
	T MinComponent(const Vector2<T>& v)
	{
		return std::min(v.x, v.y);
	}
	template <typename T>
	T MinComponent(const Vector3<T>& v)
	{
		return std::min(v.x, std::min(v.y, v.z));
	}
	template <typename T>
	T MaxComponent(const Vector2<T>& v)
	{
		return std::max(v.x, v.y);
	}
	template <typename T>
	T MaxComponent(const Vector3<T>& v)
	{
		return std::max(v.x, std::max(v.y, v.z));
	}
	template <typename T>
	int MaxDimension(const Vector2<T>& v)
	{
		return (v.x > v.y) ? 0 : 1;
	}
	template <typename T>
	int MaxDimension(const Vector3<T>& v)
	{
		return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
	}

	template <typename T>
	Point2<T> Min(const Point2<T>& p1, const Point2<T>& p2)
	{
		return Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
	}
	template <typename T>
	Point3<T> Min(const Point3<T>& p1, const Point3<T>& p2)
	{
		return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
	}
	template <typename T>
	Vector2<T> Min(const Vector2<T>& v1, const Vector2<T>& v2)
	{
		return Vector2<T>(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
	}
	template <typename T>
	Vector3<T> Min(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		return Vector3<T>(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
	}
	template <typename T>
	Point2<T> Max(const Point2<T>& p1, const Point2<T>& p2)
	{
		return Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
	}
	template <typename T>
	Point3<T> Max(const Point3<T>& p1, const Point3<T>& p2)
	{
		return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
	}
	template <typename T>
	Vector2<T> Max(const Vector2<T>& v1, const Vector2<T>& v2)
	{
		return Vector2<T>(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
	}
	template <typename T>
	Vector3<T> Max(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		return Vector3<T>(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
	}

	// operators

	template <typename T>
	inline Point2<T> operator*(T s, const Point2<T>& p)
	{
		return p * s;
	}
	template <typename T>
	inline Point3<T> operator*(T s, const Point3<T>& p)
	{
		return p * s;
	}
	template <typename T, typename U>
	inline Vector2<T> operator*(U s, const Vector2<T>& v)
	{
		return v * s;
	}
	template <typename T, typename U>
	inline Vector3<T> operator*(U s, const Vector3<T>& v)
	{
		return v * s;
	}
	template <typename T, typename U>
	inline Normal3<T> operator*(U s, const Normal3<T>& n)
	{
		return n * s;
	}

	// Permute

	template <typename T>
	Point2<T> Permute(const Point2<T>& p, int x, int y)
	{
		return Point2<T>(p[x], p[y]);
	}
	template <typename T>
	Point3<T> Permute(const Point3<T>& p, int x, int y, int z)
	{
		return Point3<T>(p[x], p[y], p[z]);
	}
	template <typename T>
	Vector2<T> Permute(const Vector2<T>& v, int x, int y)
	{
		return Vector2<T>(v[x], v[y]);
	}
	template <typename T>
	Vector3<T> Permute(const Vector3<T>& v, int x, int y, int z)
	{
		return Vector3<T>(v[x], v[y], v[z]);
	}

	// Faceforward

	template <typename T>
	inline Normal3<T> Faceforward(const Normal3<T>& n, const Vector3<T>& v)
	{
		return (Dot(n, v) < 0.f) ? -n : n;
	}
	template <typename T>
	inline Normal3<T> Faceforward(const Normal3<T>& n1, const Normal3<T>& n2)
	{
		return (Dot(n1, n2) < 0.f) ? -n1 : n2;
	}
	template <typename T>
	inline Vector3<T> Faceforward(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		return (Dot(v1, v2) < 0.f) ? -v1 : v2;
	}
	template <typename T>
	inline Vector3<T> Faceforward(const Vector3<T>& v, const Normal3<T>& n)
	{
		return (Dot(v, n) < 0.f) ? -v : n;
	}

	// Lerps

	template <typename T>
	Point2<T> Lerp(Float t, const Point2<T>& p0, const Point2<T>& p1)
	{
		return (1 - t) * p0 + t * p1;
	}
	template <typename T>
	Point3<T> Lerp(Float t, const Point3<T>& p0, const Point3<T>& p1)
	{
		return (1 - t) * p0 + t * p1;
	}

	// Distances

	template <typename T>
	inline Float Distance(const Point2<T>& p1, const Point2<T>& p2)
	{
		return (p1 - p2).Length();
	}
	template <typename T>
	inline Float Distance(const Point3<T>& p1, const Point3<T>& p2)
	{
		return (p1 - p2).Length();
	}
	template <typename T, typename U>
	inline Float Distance(const Point3<T>& p, const Bounds3<U>& b)
	{
		return std::sqrt(DistanceSquared(p, b));
	}
	template <typename T>
	inline Float DistanceSquared(const Point2<T>& p1, const Point2<T>& p2)
	{
		return (p1 - p2).LengthSquared();
	}
	template <typename T>
	inline Float DistanceSquared(const Point3<T>& p1, const Point3<T>& p2)
	{
		return (p1 - p2).LengthSquared();
	}
	// Minimum squared distance from point to box; zero if point is inside.
	template <typename T, typename U>
	inline Float DistanceSquared(const Point3<T>& p, const Bounds3<U>& b)
	{
		Float dx = std::max({ Float(0), b.pMin.x - p.x, p.x - b.pMax.x });
		Float dy = std::max({ Float(0), b.pMin.y - p.y, p.y - b.pMin.y });
		Float dz = std::max({ Float(0), b.pMin.z - p.z, p.z - b.pMax.z });
		return dx * dx + dy * dy + dz * dz;
	}

	// Others

	template <typename T>
	inline Vector2<T> Normalize(const Vector2<T>& v)
	{
		return v / v.Length();
	}
	template <typename T>
	inline Vector3<T> Normalize(const Vector3<T>& v)
	{
		return v / v.Length();
	}
	template <typename T>
	inline Normal3<T> Normalize(const Normal3<T>& n)
	{
		return n / n.Length();
	}
	template <typename T>
	Point2<T> Floor(const Point2<T>& p)
	{
		return Point2<T>(std::floor(p.x), std::floor(p.y));
	}
	template <typename T>
	Point3<T> Floor(const Point3<T>& p)
	{
		return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z));
	}
	template <typename T>
	Point2<T> Ceil(const Point2<T>& p)
	{
		return Point2<T>(std::ceil(p.x), std::ceil(p.y));
	}
	template <typename T>
	Point3<T> Ceil(const Point3<T>& p)
	{
		return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z));
	}
	template <typename T>
	inline void CoordinateSystem(const Vector3<T>& v1, Vector3<T>* v2, Vector3<T>* v3)
	{
		if (std::abs(v1.x) > std::abs(v1.y))
			*v2 = Vector3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
		else
			*v2 = Vector3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
		*v3 = Cross(v1, *v2);
	}

	// Bounds intersection

	template <typename T>
	inline bool Bounds3<T>::IntersectP(const Ray& ray, Float* hitt0, Float* hitt1) const
	{
		Float t0 = 0, t1 = ray.tMax;
		for (int i = 0; i < 3; ++i)
		{
			// Update interval for the 'i'th bounding box slab
			Float invRayDir = 1 / ray.d[i];
			Float tNear = (pMin[i] - ray.o[i]) * invRayDir;
			Float tFar = (pMax[i] - ray.o[i]) * invRayDir;

			// Update parametric interval from slab intersection 't' values
			if (tNear > tFar) std::swap(tNear, tFar);

			// Update 'tFar' to ensure robust ray-bounds intersection
			tFar *= 1 + 2 * gamma(3);
			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1) return false;
		}
		if (hitt0) *hitt0 = t0;
		if (hitt1) *hitt1 = t1;
		return true;
	}
	template <typename T>
	inline bool Bounds3<T>::IntersectP(const Ray& ray, const Vector3f& invDir, const int dirIsNeg[3]) const
	{
		const Bounds3f& bounds = *this;
		// Check for ray intersection agains 'x' and 'y' slabs
		Float tMin = (bounds[dirIsNeg[0]].x - ray.o.x) * invDir.x;
		Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
		Float tyMin = (bounds[dirIsNeg[1]].y - ray.o.y) * invDir.y;
		Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;

		// Update tMax and tyMax to ensure robust bounds intersection
		tMax *= 1 + 2 * gamma(3);
		tyMax *= 1 + 2 * gamma(3);
		if (tMin > tyMax || tyMin > tMax) return false;
		if (tyMin > tMin) tMin = tyMin;
		if (tyMax < tMax) tMax = tyMax;

		// Check for ray intersection against 'z' slab
		Float tzMin = (bounds[dirIsNeg[2]].z - ray.o.z) * invDir.z;
		Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z;

		// Update 'tzMax' to ensure robust bounds intersection
		tzMax *= 1 + 2 * gamma(3);
		if (tMin > tzMax || tzMin > tMax) return false;
		if (tzMin > tMin) tMin = tzMin;
		if (tzMax < tMax) tMax = tzMax;
		return (tMin < ray.tMax) && (tMax > 0);
	}


} // namespace graphics