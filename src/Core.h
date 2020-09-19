#pragma once
#include <cassert>

#ifndef expect
#define expect(x) assert(x)
#endif

template<class T>
bool areEqual(T x, T y)
{
    static constexpr const double EPSILON = 1e-2;
    return std::fabs(x - y) < EPSILON;
}

template<typename T>
bool isLess(T a, T b)
{
    return !areEqual(a, b) && a < b;
}

template<typename T>
bool isGreater(T a, T b)
{
    return !areEqual(a, b) && a > b;
}