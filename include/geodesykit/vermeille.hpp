// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_VERMEILLE_HPP
#define GEODESYKIT_VERMEILLE_HPP

#include "constants.hpp"
#include <cmath>
#include <limits>

namespace geodesykit {

/// Convert ECEF (x, y, z) to geodetic (latitude, longitude, height).
///
/// Uses a guarded iterative solution. Invalid ellipsoids, non-finite inputs,
/// and the Earth centre (where latitude/longitude are undefined) produce NaNs.
/// Latitude and longitude are returned in degrees; height is in metres.
inline void ecef2geodetic(double x, double y, double z,
                          double& latDeg, double& lonDeg, double& h,
                          double a = WGS84_A, double e2 = WGS84_E2) noexcept
{
    const double qnan = std::numeric_limits<double>::quiet_NaN();
    const auto fail = [&]() noexcept { latDeg = lonDeg = h = qnan; };

    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) ||
        !std::isfinite(a) || !std::isfinite(e2) || a <= 0.0 ||
        e2 < 0.0 || e2 >= 1.0) {
        fail();
        return;
    }

    const double p = std::hypot(x, y);
    const double radius = std::hypot(p, z);
    if (radius < 1e-9) { // No unique geodetic latitude/longitude at the centre.
        fail();
        return;
    }

    const double b = a * std::sqrt(1.0 - e2);
    if (p < 1e-12) {
        latDeg = std::copysign(90.0, z);
        lonDeg = 0.0;
        h = std::fabs(z) - b;
        return;
    }

    const double lon = std::atan2(y, x);
    double lat = std::atan2(z, p * (1.0 - e2));
    double n = a;
    bool converged = false;

    // Fixed-point iteration is stable for ordinary exterior points and avoids
    // the 0/0 branch in the closed-form cubic near the ellipsoid's evolute.
    for (int i = 0; i < 20; ++i) {
        const double s = std::sin(lat);
        const double denom = 1.0 - e2 * s * s;
        if (!(denom > 0.0) || !std::isfinite(denom)) {
            fail();
            return;
        }
        n = a / std::sqrt(denom);
        const double next = std::atan2(z + e2 * n * s, p);
        if (!std::isfinite(next)) {
            fail();
            return;
        }
        if (std::fabs(next - lat) <= 2.0 * std::numeric_limits<double>::epsilon()) {
            lat = next;
            converged = true;
            break;
        }
        lat = next;
    }

    const double s = std::sin(lat);
    const double c = std::cos(lat);
    n = a / std::sqrt(1.0 - e2 * s * s);
    // Select the altitude expression whose denominator is better conditioned.
    const double height = (std::fabs(c) >= std::fabs(s))
        ? p / c - n
        : z / s - n * (1.0 - e2);

    if (!converged || !std::isfinite(height)) {
        // For deep interior points, geodetic coordinates may be non-unique.
        // Return a visible failure rather than silently emitting corrupt data.
        fail();
        return;
    }

    latDeg = lat * RAD2DEG;
    lonDeg = lon * RAD2DEG;
    h = height;
}

} // namespace geodesykit
#endif // GEODESYKIT_VERMEILLE_HPP
