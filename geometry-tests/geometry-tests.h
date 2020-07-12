#pragma once

#include "graphics.h"
#include "geometry.h"
#include "gtest/gtest.h"

using namespace graphics;

// Vector tests

class Vector2Test : public ::testing::Test
{
protected:
    // void SetUp() override {}
    // void TearDown() override {}

    bool equal(Vector2<int>& v1, Vector2<int>& v2)
    {
        return v1.x == v2.x && v1.y == v2.y;
    }

    void reset_i_() { i_.x = 1; i_.y = 0; }

    Vector2<int> v;

    Vector2<int> o{ 0, 0 };
    Vector2<int> i{ 1, 0 };
    Vector2<int> j{ 0, 1 };

    Vector2<int> _i{ -1, 0 }; // -i
    Vector2<int> i_{ 1, 0 };  // temp i

    Vector2<int> i_plus_j{ 1, 1 };   // i + j
    Vector2<int> i_minus_j{ 1, -1 }; // i - j
};

class Vector3Test : public ::testing::Test
{
protected:
    // void SetUp() override {}
    // void TearDown() override {}

    bool equal(Vector3<int>& v1, Vector3<int>& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }

    void reset_i_() { i_.x = 1; i_.y = 0; i_.z = 0; }

    Vector3<int> v;

    Vector3<int> o{ 0, 0, 0 };
    Vector3<int> i{ 1, 0, 0 };
    Vector3<int> j{ 0, 1, 0 };
    Vector3<int> k{ 0, 0, 1 };

    Vector3<int> _i{ -1, 0, 0 }; // -i
    Vector3<int> i_{ 1, 0, 0 };  // temp i

    Vector3<int> i_plus_j{ 1, 1, 0 };   // i + j
    Vector3<int> i_minus_j{ 1, -1, 0 }; // i - j
};

// Point tests

class Point2Test : public ::testing::Test
{
protected:
    // void SetUp() override {}
    // void TearDown() override {}

    bool equal(Point2<int>& p1, Point2<int>& p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }
    bool equal_f(Point2<Float>& p1, Point2<Float>& p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }
    bool equal(Vector2<int>& v1, Vector2<int>& v2)
    {
        return v1.x == v2.x && v1.y == v2.y;
    }

    void reset_o() { o.x = o.y = 0; }
    void reset_p1() { p1.x = -1;  p1.y = 0; }

    Point2<int>  p;
    Point2<int>  o{  0, 0 };
    Point2<int> p1{ -1, 0 };
    Point2<int> p2{  0, 1 };
    Point2<Float> p1_f{ -1.0, 0.0 };
    Point2<Float> p2_f{  0.0, 1.0 };
    Vector2<int> v{ 0, 1 };
};

class Point3Test : public ::testing::Test
{
protected:
    // void SetUp() override {}
    // void TearDown() override {}

    bool equal(Point3<int>& p1, Point3<int>& p2)
    {
        return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
    }
    bool equal_f(Point3<Float>& p1, Point3<Float>& p2)
    {
        return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
    }
    bool equal(Vector3<int>& v1, Vector3<int>& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }

    void reset_o() { o.x = o.y = o.z = 0; }
    void reset_p1() { p1.x = -1;  p1.y = 0;  p1.z = 1; }

    Point3<int>  p;
    Point3<int>  o{  0, 0,  0 };
    Point3<int> p1{ -1, 0,  1 };
    Point3<int> p2{  0, 1, -1 };
    Point3<Float> p1_f{ -1.0, 0.0,  1.0 };
    Point3<Float> p2_f{  0.0, 1.0, -1.0 };
    Vector3<int> v{ 0, 1, -1 };
};

// Normal tests

class Normal3Test : public ::testing::Test
{
protected:
    // void SetUp() override {}
    // void TearDown() override {}

    bool equal(Normal3<int>& n1, Normal3<int>& n2)
    {
        return n1.x == n2.x && n1.y == n2.y && n1.z == n2.z;
    }

    void reset_i_() { i_.x = 1; i_.y = 0; i_.z = 0; }

    Normal3<int> n;

    Normal3<int> o{ 0, 0, 0 };
    Normal3<int> i{ 1, 0, 0 };
    Normal3<int> j{ 0, 1, 0 };
    Normal3<int> k{ 0, 0, 1 };

    Normal3<int> _i{ -1, 0, 0 }; // -i
    Normal3<int> i_{ 1, 0, 0 };  // temp i

    Normal3<int> i_plus_j{ 1, 1, 0 };   // i + j
    Normal3<int> i_minus_j{ 1, -1, 0 }; // i - j
};
