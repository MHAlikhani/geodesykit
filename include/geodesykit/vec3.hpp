// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#ifndef GEODESYKIT_VEC3_HPP
#define GEODESYKIT_VEC3_HPP

#include <cmath>

namespace geodesykit {

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

inline double dot(const Vec3& a, const Vec3& b) noexcept
{
    return std::fma(a.x, b.x, std::fma(a.y, b.y, a.z * b.z));
}

inline double norm(const Vec3& v) noexcept
{
    return std::hypot(v.x, v.y, v.z);
}

inline Vec3 normalize(const Vec3& v) noexcept
{
    const double n = norm(v);
    if (n < 1e-15) return Vec3{0.0, 0.0, 0.0};
    const double inv = 1.0 / n;
    return Vec3{v.x * inv, v.y * inv, v.z * inv};
}

inline Vec3 sub(const Vec3& a, const Vec3& b) noexcept
{
    return Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 add(const Vec3& a, const Vec3& b) noexcept
{
    return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 mul(const Vec3& v, double s) noexcept
{
    return Vec3{v.x * s, v.y * s, v.z * s};
}

} // namespace geodesykit

#endif // GEODESYKIT_VEC3_HPP