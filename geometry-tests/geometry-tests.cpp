//#include "graphics.h"
#include "geometry-tests.h"

// Point tests

TEST_F(Point2Test, Initializations)
{
    ASSERT_TRUE(equal(p, o));
}

TEST_F(Point2Test, Operations)
{
    EXPECT_TRUE(equal(p1 + p2, Point2<int>{-1, 1}));
    EXPECT_TRUE(equal(p1 += p2, Point2<int>{-1, 1})); reset_p1();
    EXPECT_TRUE(equal(p1 + v, Point2<int>{-1, 1}));
    EXPECT_TRUE(equal(p1 += v, Point2<int>{-1, 1})); reset_p1();

    EXPECT_TRUE(equal(p1 - p2, Vector2<int>{-1, -1}));
    EXPECT_TRUE(equal(p1 - v, Point2<int>{-1, -1}));
    EXPECT_TRUE(equal(p1 -= v, Point2<int>{-1, -1})); reset_p1();

    EXPECT_TRUE(equal(p1 * 0, o));
    EXPECT_TRUE(equal(0 * p1, o));
    EXPECT_TRUE(equal(p1 *= 0, o)); reset_p1();

    EXPECT_FLOAT_EQ(Distance(p1, p2), 1.414213562); // sqrt(2)
    EXPECT_EQ(DistanceSquared(p1, p2), 2);
    EXPECT_TRUE(equal_f(Lerp(0.5, p1_f, p2_f), Point2<Float>{-0.5, 0.5}));

    EXPECT_TRUE(equal(Abs(p1), Point2<int>{ 1, 0}));
    EXPECT_TRUE(equal(Max(p1, p2), Point2<int>{ 0, 1}));
    EXPECT_TRUE(equal(Min(p1, p2), Point2<int>{-1, 0}));
    EXPECT_TRUE(equal(Permute(p1, 1, 0), Point2<int>{ 0, -1}));

    EXPECT_TRUE(equal_f(Floor(Point2<Float>{-0.5, 0.5}), Point2<Float>{-1.0, 0.0}));
    EXPECT_TRUE(equal_f(Ceil(Point2<Float>{-0.5, 0.5}), Point2<Float>{ 0.0, 1.0}));
}

TEST_F(Point3Test, Initializations)
{
    ASSERT_TRUE(equal(p, o));
}

TEST_F(Point3Test, Operations)
{
    EXPECT_TRUE(equal(p1 + p2, Point3<int>{-1, 1, 0}));
    EXPECT_TRUE(equal(p1 += p2, Point3<int>{-1, 1, 0})); reset_p1();
    EXPECT_TRUE(equal(p1 + v, Point3<int>{-1, 1, 0}));
    EXPECT_TRUE(equal(p1 += v, Point3<int>{-1, 1, 0})); reset_p1();

    EXPECT_TRUE(equal(p1 - p2, Vector3<int>{-1, -1, 2}));
    EXPECT_TRUE(equal(p1 - v, Point3<int>{-1, -1, 2}));
    EXPECT_TRUE(equal(p1 -= v, Point3<int>{-1, -1, 2})); reset_p1();

    EXPECT_TRUE(equal(p1 * 0, o));
    EXPECT_TRUE(equal(0 * p1, o));
    EXPECT_TRUE(equal(p1 *= 0, o)); reset_p1();

    EXPECT_FLOAT_EQ(Distance(p1, p2), 2.449489743); // sqrt(6)
    EXPECT_EQ(DistanceSquared(p1, p2), 6);
    EXPECT_TRUE(equal_f(Lerp(0.5, p1_f, p2_f), Point3<Float>{-0.5, 0.5, 0.0}));

    EXPECT_TRUE(equal(Abs(p1), Point3<int>{ 1, 0, 1}));
    EXPECT_TRUE(equal(Max(p1, p2), Point3<int>{ 0, 1, 1}));
    EXPECT_TRUE(equal(Min(p1, p2), Point3<int>{-1, 0, -1}));
    EXPECT_TRUE(equal(Permute(p1, 1, 2, 0), p2));

    EXPECT_TRUE(equal_f(Floor(Point3<Float>{-0.5, 0.0, 0.5}), Point3<Float>{-1.0, 0.0, 0.0}));
    EXPECT_TRUE(equal_f(Ceil(Point3<Float>{-0.5, 0.0, 0.5}), Point3<Float>{0.0, 0.0, 1.0}));
}

// Vector tests

TEST_F(Vector2Test, Initializations)
{
    ASSERT_TRUE(equal(v, o));  // default construction
    ASSERT_TRUE(equal(i_, i)); // parametrized constuction
}

TEST_F(Vector2Test, Operations)
{

    EXPECT_TRUE(equal(i + j, i_plus_j));
    EXPECT_TRUE(equal(i_ += j, i_plus_j)); reset_i_();


    EXPECT_TRUE(equal(i - j, i_minus_j));
    EXPECT_TRUE(equal(i_ -= j, i_minus_j)); reset_i_();


    EXPECT_TRUE(equal(i * 0, o));
    EXPECT_TRUE(equal(i_ *= 0, o)); reset_i_();


    ASSERT_TRUE(equal(-i, _i));

    EXPECT_TRUE(equal(i / -1, -i));
    EXPECT_TRUE(equal(i_ /= -1, -i)); reset_i_();

    EXPECT_EQ(Dot(i, j), 0);

    EXPECT_TRUE(i[0] == 1 && i[1] == 0 && i[1] == 0);

    EXPECT_EQ(i.Length(), 1);
    EXPECT_EQ(i.LengthSquared(), 1);

    EXPECT_TRUE(equal(Abs(-i), i));
    EXPECT_EQ(AbsDot(i, -i), 1);

    EXPECT_TRUE(equal(Max(i, j), i_plus_j));
    EXPECT_TRUE(equal(Min(i, j), o));

    EXPECT_EQ(MaxComponent(i), 1);
    EXPECT_EQ(MinComponent(i), 0);

    EXPECT_EQ(MaxDimension(j), 1);

    EXPECT_TRUE(equal(Permute(i, 1, 0), j));
    EXPECT_TRUE(equal(Normalize(i), i));
}

TEST_F(Vector3Test, Initializations)
{
    ASSERT_TRUE(equal(v, o));  // default construction
    ASSERT_TRUE(equal(i_, i)); // parametrized constuction
}

TEST_F(Vector3Test, Operations)
{

    EXPECT_TRUE(equal(i + j, i_plus_j));
    EXPECT_TRUE(equal(i_ += j, i_plus_j)); reset_i_();


    EXPECT_TRUE(equal(i - j, i_minus_j));
    EXPECT_TRUE(equal(i_ -= j, i_minus_j)); reset_i_();


    EXPECT_TRUE(equal(i * 0, o));
    EXPECT_TRUE(equal(0 * i, o));
    EXPECT_TRUE(equal(i_ *= 0, o)); reset_i_();


    ASSERT_TRUE(equal(-i, _i));

    EXPECT_TRUE(equal(i / -1, -i));
    EXPECT_TRUE(equal(i_ /= -1, -i)); reset_i_();

    EXPECT_EQ(Dot(i, j), 0);
    EXPECT_TRUE(equal(Cross(i, j), k));

    EXPECT_TRUE(i[0] == 1 && i[1] == 0 && i[1] == 0);

    EXPECT_EQ(i.Length(), 1);
    EXPECT_EQ(i.LengthSquared(), 1);

    EXPECT_TRUE(equal(Abs(-i), i));
    EXPECT_EQ(AbsDot(i, -i), 1);

    EXPECT_TRUE(equal(Max(i, j), i_plus_j));
    EXPECT_TRUE(equal(Min(i, j), o));

    EXPECT_EQ(MaxComponent(i), 1);
    EXPECT_EQ(MinComponent(i), 0);

    EXPECT_EQ(MaxDimension(k), 2);

    EXPECT_TRUE(equal(Permute(i, 1, 2, 0), k));
    EXPECT_TRUE(equal(Normalize(i), i));

    Vector3<int> v2, v3;
    CoordinateSystem(i, &v2, &v3);
    EXPECT_TRUE(equal(v2, k));
    EXPECT_TRUE(equal(v3, -j));
}

// Normal tests

TEST_F(Normal3Test, Initializations)
{
    ASSERT_TRUE(equal(n, o));  // default construction
    ASSERT_TRUE(equal(i_, i)); // parametrized constuction
}

TEST_F(Normal3Test, Operations)
{
    EXPECT_TRUE(equal(i + j, i_plus_j));
    EXPECT_TRUE(equal(i_ += j, i_plus_j));
    reset_i_();

    EXPECT_TRUE(equal(i - j, i_minus_j));
    EXPECT_TRUE(equal(i_ -= j, i_minus_j));
    reset_i_();

    EXPECT_TRUE(equal(i * 0, o));
    EXPECT_TRUE(equal(0 * i, o));
    EXPECT_TRUE(equal(i_ *= 0, o));
    reset_i_();


    ASSERT_TRUE(equal(-i, _i));

    EXPECT_TRUE(equal(i / -1, -i));
    EXPECT_TRUE(equal(i_ /= -1, -i));
    reset_i_();

    EXPECT_EQ(Dot(i, j), 0);

    EXPECT_TRUE(i[0] == 1 && i[1] == 0 && i[1] == 0);

    EXPECT_EQ(i.Length(), 1);
    EXPECT_EQ(i.LengthSquared(), 1);

    EXPECT_TRUE(equal(Abs(-i), i));
    EXPECT_EQ(AbsDot(i, -i), 1);

    EXPECT_TRUE(equal(Normalize(i), i));
}

