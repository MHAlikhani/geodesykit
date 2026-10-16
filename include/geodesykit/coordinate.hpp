// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_COORDINATE_HPP
#define GEODESYKIT_COORDINATE_HPP

#include "constants.hpp"
#include "vec3.hpp"
#include "vermeille.hpp"
#include <cmath>
#include <limits>

namespace geodesykit {

inline bool validEllipsoid(double a, double e2) noexcept
{
    return std::isfinite(a) && std::isfinite(e2) && a > 0.0 && e2 >= 0.0 && e2 < 1.0;
}

/// Convert geodetic coordinates (degrees, degrees, metres) to ECEF metres.
/// Invalid inputs produce a Vec3 whose components are quiet NaNs.
inline Vec3 geodetic2ecef(double latDeg, double lonDeg, double h,
                          double a = WGS84_A, double e2 = WGS84_E2) noexcept
{
    const double qnan = std::numeric_limits<double>::quiet_NaN();
    if (!validEllipsoid(a, e2) || !std::isfinite(latDeg) ||
        !std::isfinite(lonDeg) || !std::isfinite(h) ||
        latDeg < -90.0 || latDeg > 90.0) {
        return Vec3{qnan, qnan, qnan};
    }

    const double lat = latDeg * DEG2RAD;
    const double lon = std::remainder(lonDeg, 360.0) * DEG2RAD;
    const double sLat = std::sin(lat);
    const double cLat = std::cos(lat);
    const double n = a / std::sqrt(1.0 - e2 * sLat * sLat);
    return Vec3{
        (n + h) * cLat * std::cos(lon),
        (n + h) * cLat * std::sin(lon),
        (n * (1.0 - e2) + h) * sLat
    };
}

/// Compatibility alias for the ECEF-to-geodetic conversion.
inline void ecef2geodetic_v(double x, double y, double z,
                            double& latDeg, double& lonDeg, double& h) noexcept
{
    ecef2geodetic(x, y, z, latDeg, lonDeg, h);
}

/// Compute the orthonormal East, North, Up basis at a geodetic location.
/// Invalid coordinates produce NaN components in all three axes.
inline void enuAxes(double latDeg, double lonDeg,
                    Vec3& E, Vec3& N, Vec3& U) noexcept
{
    const double qnan = std::numeric_limits<double>::quiet_NaN();
    if (!std::isfinite(latDeg) || !std::isfinite(lonDeg) ||
        latDeg < -90.0 || latDeg > 90.0) {
        E = N = U = Vec3{qnan, qnan, qnan};
        return;
    }
    const double lat = latDeg * DEG2RAD;
    const double lon = std::remainder(lonDeg, 360.0) * DEG2RAD;
    const double sLat = std::sin(lat), cLat = std::cos(lat);
    const double sLon = std::sin(lon), cLon = std::cos(lon);
    U = Vec3{cLat * cLon, cLat * sLon, sLat};
    E = Vec3{-sLon, cLon, 0.0};
    N = Vec3{-sLat * cLon, -sLat * sLon, cLat};
}

} // namespace geodesykit
#endif // GEODESYKIT_COORDINATE_HPP
