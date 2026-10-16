// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_VEC3_HPP
#define GEODESYKIT_VEC3_HPP
#include <cmath>
namespace geodesykit {
struct Vec3 { double x = 0.0; double y = 0.0; double z = 0.0; };
inline double dot(const Vec3& a, const Vec3& b) noexcept {
#if defined(__FMA__) || defined(__AVX2__)
    return std::fma(a.x, b.x, std::fma(a.y, b.y, a.z * b.z));
#else
    return a.x*b.x + a.y*b.y + a.z*b.z;
#endif
}
inline double norm(const Vec3& v) noexcept { return std::hypot(v.x, v.y, v.z); }
inline Vec3 normalize(const Vec3& v) noexcept {
    const double n = norm(v);
    if (!std::isfinite(n) || n < 1e-15) return Vec3{0.0, 0.0, 0.0};
    return Vec3{v.x/n, v.y/n, v.z/n};
}
inline Vec3 sub(const Vec3& a, const Vec3& b) noexcept { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
inline Vec3 add(const Vec3& a, const Vec3& b) noexcept { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
inline Vec3 mul(const Vec3& v, double s) noexcept { return {v.x*s, v.y*s, v.z*s}; }
}
#endif
