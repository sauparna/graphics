#pragma once

// <Vector Declarations>

template <typename T> class Vector2
{
public:
	// <Vector2 Public Methods>
	// <Vector2 Public Data>
	T x, y;
};

template <typename T> class Vector3
{
public:
	// <Vector3 Public Methods>
	Vector3() { T x = y = z = 0; };
	Vector3(T x, T y, T z) : x{ 0 }, y{ 0 }, z{ 0 } { assert(!HasNaNs()); }
	bool HasNaNs() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }
	Float LengthSquared() const { return x * x + y * y + z * z; }
	Float Length() const { return std::sqrt(LengthSquared()); }
	T operator[](int i) const
	{
		assert(i >= 0 && i <= 2);
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}
	T& operator[](int i) const
	{
		assert(i >= 0 && i <= 2);
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}
	Vector3<T> operator+(const Vector3<T>& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3<T>& operator+=(const Vector3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3<T> operator-(const Vector3<T>& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3<T>& operator-=(const Vector3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3<T> operator*(T s) const { return Vector3<T>(x * s, y * s, z * s); }
	Vector3<T>& operator*=(T s) { x *= s; y *= s; z *= s; return *this; }
	Vector3<T> operator/(T s) const
	{
		assert(s != 0);
		Float inv = static_cast<Float>(1) / s;
		return Vector3<T>(x * inv, y * inv, z * inv);
	}
	Vector3<T>& operator/=(T s)
	{
		assert(s != 0);
		Float inv = static_cast<Float>(1) / s;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }

	// <Vector3 Public Data>
	T x, y, z;
};

typedef Vector3<Float> Vector3f;
typedef Vector3<int> Vector3i;

// <Geometry Inline Functions>
template <typename T>
inline Vector3<T> operator*(T s, const Vector3<T>& v) { return v * s; }

template <typename T>
inline Vector3<T> Abs(const Vector3<T>& v) { return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z)); }

template <typename T>
inline T Dot(const Vector3<T>& v1, const Vector3<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
inline T AbsDot(const Vector3<T>& v1, const Vector3<T>& v2)
{
	return std::abs(Dot(v1, v2));
}

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
template <typename T>
inline Vector3<T> Normalize(const Vector3<T>& v) { return v / v.Length(); }
template <typename T>
T MinComponent(const Vector3<T>& v) { return std::min(v.x, std::min(v.y, v.z)); }
template <typename T>
T MaxComponent(const Vector3<T>& v) { return std::max(v.x, std::max(v.y, v.z)); }
template <typename T>
int MaxDimension(const Vector3<T>& v)
{
	return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}
template <typename T>
Vector3<T> Min(const Vector3<T>& v1, const Vector3<T>& v2)
{
	return Vector3<T>(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}
template <typename T>
Vector3<T> Max(const Vector3<T>& v1, const Vector3<T>& v2)
{
	return Vector3<T>(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}
template <typename T>
Vector3<T> Permute(const Vector3<T>& v, int x, int y, int z) { return Vector3<T>(v[x], v[y], v[z]); }
template <typename T>
inline void CoordinateSystem(const Vector3<T>& v1, Vector3<T>* v2, Vector3<T>* v3)
{
	if (std::abs(v1.x) > std::abs(v1.y))
		*v2 = Vector3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
	else
		*v2 = Vector3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
	*v3 = Cross(v2, *v2);
}

// <Point Declarations>

template <typename T>
class Point3
{
public:
	// <Point Public Methods>
	Point3() { x = y = z = 0; }
	Point3(T x, T y, T z) : x{ x }, y{ y }, z{ z } { assert(!HasNaNs()); }
	bool HasNaNs() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }
	template <typename U>
	explicit Point3(const Point3<U>& p) : x{(T)p.x}, y{(T)p.y}, z{(T)p.z} { assert(!HasNaNs()); }
	template <typename U>
	explicit operator Vector3<U>() const { return Vector3<U>(x, y, z); }
	Point3<T> operator+(const Vector3<T>& v) const { return Point3<T>(x + v.x, y + v.y, z + v.z); }
	Point3<T>& operator+=(const Vector3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Point3<T> operator-(const Vector3<T>& v) const { return Point3<T>(x - v.x, y - v.y, z - v.z); }
	Point3<T> operator-=(const Vector3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3<T> operator-(const Point3<T>& p) const { return Vector3<T>(x - p.x, y - p.y, z - p.z); }
	Point3<T> operator+(const Point3<T>& p) const { return Point3<T>(x + p.x, y + p.y, z + p.z); }
	Point3<T>& operator+=(const Point3<T>& p) { x += p.x; y += p.y; z += p.z; return *this; }
	Point3<T> operator*(T s) const { return Point3<T>(x * s, y * s, z * s); }
	Point3<T>& operator*=(T s) { x *= s; y *= s; z *= s; return *this; }
	// <Point Public Data>
	T x, y, z;
};

typedef Point3<Float> Point3f;
typedef Point3<int> Point3i;

template <typename T>
inline Point3<T> operator*(T s, const Point3<T>& p) { return p * s; }
template <typename T>
inline Float Distance(const Point3<T>& p1, const Point3<T>& p2) { return (p1 - p2).Length(); }
template <typename T>
inline Float DistanceSquared(const Point3<T>& p1, const Point3<T>& p2) { return (p1 - p2).LengthSquared(); }
template <typename T>
Point3<T> Lerp(Float t, const Point3<T>& p0, Point3<T>& p1) { return (1 - t) * p0 + t * p1; }
template <typename T>
Point3<T> Min(const Point3<T>& p1, const Point3<T>& p2)
{
	return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
}
template <typename T>
Point3<T> Max(const Point3<T>& p1, const Point3<T>& p2)
{
	return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
}
template <typename T>
Point3<T> Floor(const Point3<T>& p) { return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z)); }
template <typename T>
Point3<T> Ceil(const Point3<T>& p) { return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z)); }
template <typename T>
Point3<T> Abs(const Point3<T>& p) { return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z)); }
template <typename T>
Point3<T> Permute(const Point3<T>& p, int x, int y, int z) { return Point3<T>(p[x], p[y], p[z]); }
