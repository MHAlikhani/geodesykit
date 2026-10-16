// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#ifndef GEODESYKIT_COORDINATE_HPP
#define GEODESYKIT_COORDINATE_HPP

#include "constants.hpp"
#include "vec3.hpp"
#include "vermeille.hpp"
#include <cmath>

namespace geodesykit {

/// Convert geodetic (lat, lon, h) to ECEF (x, y, z).
inline Vec3 geodetic2ecef(double latDeg, double lonDeg, double h,
                          double a = WGS84_A, double e2 = WGS84_E2) noexcept
{
    const double lat  = latDeg * DEG2RAD;
    const double lon  = lonDeg * DEG2RAD;
    const double sLat = std::sin(lat);
    const double cLat = std::cos(lat);
    const double N    = a / std::sqrt(1.0 - e2 * sLat * sLat);

    return Vec3{
        (N + h) * cLat * std::cos(lon),
        (N + h) * cLat * std::sin(lon),
        (N * (1.0 - e2) + h) * sLat
    };
}

/// Convert ECEF (x, y, z) to geodetic (lat, lon, h) using Vermeille.
inline void ecef2geodetic_v(double x, double y, double z,
                            double& latDeg, double& lonDeg, double& h) noexcept
{
    ecef2geodetic(x, y, z, latDeg, lonDeg, h);
}

/// Compute local ENU axes at a given geodetic location.
inline void enuAxes(double latDeg, double lonDeg,
                    Vec3& E, Vec3& N, Vec3& U) noexcept
{
    const double lat  = latDeg * DEG2RAD;
    const double lon  = lonDeg * DEG2RAD;
    const double sLat = std::sin(lat), cLat = std::cos(lat);
    const double sLon = std::sin(lon), cLon = std::cos(lon);

    U = Vec3{ cLat * cLon,  cLat * sLon,  sLat};
    E = Vec3{-sLon,         cLon,         0.0};
    N = Vec3{-sLat * cLon, -sLat * sLon,  cLat};
}

} // namespace geodesykit

#endif // GEODESYKIT_COORDINATE_HPP