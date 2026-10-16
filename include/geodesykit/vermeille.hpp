// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
//
// Implementation based on:
//   H. Vermeille, "Direct transformation from geocentric coordinates
//   to geodetic coordinates", Journal of Geodesy, 76(8):451–454, 2002.
//   H. Vermeille, "Computing geodetic coordinates from geocentric
//   coordinates", Journal of Geodesy, 78(1):94–95, 2004.

#ifndef GEODESYKIT_VERMEILLE_HPP
#define GEODESYKIT_VERMEILLE_HPP

#include "constants.hpp"
#include <cmath>

namespace geodesykit {

/// Convert ECEF (x, y, z) to geodetic (lat, lon, h) using Vermeille's method.
/// Achieves machine precision without iteration.
inline void ecef2geodetic(double x, double y, double z,
                          double& latDeg, double& lonDeg, double& h,
                          double a = WGS84_A, double e2 = WGS84_E2) noexcept
{
    const double e4 = e2 * e2;
    const double r  = std::hypot(x, y);

    // --- Longitude ---
    double lon;
    if (r < 1e-15) {
        lon = 0.0;
    } else if (y >= 0.0) {
        lon = 0.5 * PI - 2.0 * std::atan(x / (r + y));
    } else {
        lon = -0.5 * PI + 2.0 * std::atan(x / (r - y));
    }

    // --- Polar case ---
    if (r < 1e-15) {
        latDeg = (z >= 0.0) ? 90.0 : -90.0;
        lonDeg = 0.0;
        h = std::fabs(z) - a * std::sqrt(1.0 - e2);
        return;
    }

    const double p   = r * r / (a * a);
    const double q   = (1.0 - e2) * z * z / (a * a);
    const double rho = (p + q - e4) / 6.0;

    double lat, hOut;

    if (rho < 0.0) {
        // --- Region 1: inside ellipsoid ---
        const double u = std::sqrt(e4 * q);
        const double v = 2.0 * u * u + e4 * (p - q) - e4 * e4;
        const double w = 4.0 * u * (u * u + e4 * (p - q));
        const double sw = std::sqrt(w);
        const double s  = std::cbrt(std::fabs(v + sw));

        double t;
        if (v + sw >= 0.0) {
            t = s + e4 * q / (s * s);
        } else {
            const double theta = std::acos(-v / sw);
            t = 2.0 * u * std::cos(theta / 3.0);
        }

        const double t_e4 = t + e4;
        const double t3   = 3.0 * t + e4;
        lat = std::atan2(std::sqrt(t_e4) * z,
                         std::sqrt(t3) * r);

        const double sLat = std::sin(lat);
        const double N = a / std::sqrt(1.0 - e2 * sLat * sLat);
        hOut = r / std::cos(lat) - N;
    } else {
        // --- Region 2/3: outside ellipsoid ---
        const double u = e4 * p * q;
        const double v = 2.0 * u + e4 * e4 * (p - q);
        const double w = 4.0 * u * (u + e4 * (p - q));

        const double sw = std::sqrt(v * v + w);
        const double t  = (v + sw >= 0.0)
                        ? std::cbrt(v + sw)
                        : -std::cbrt(std::fabs(v + sw));

        const double u2 = t + v / (t * t);
        const double u2_e4 = u2 + e4;

        lat = std::atan2(z * (1.0 + e2) / std::sqrt(u2_e4),
                         r * std::sqrt(u2 / u2_e4));

        const double sLat = std::sin(lat);
        const double N = a / std::sqrt(1.0 - e2 * sLat * sLat);
        hOut = r / std::cos(lat) - N;
    }

    latDeg = lat * RAD2DEG;
    lonDeg = lon * RAD2DEG;
    h      = hOut;
}

} // namespace geodesykit

#endif // GEODESYKIT_VERMEILLE_HPP