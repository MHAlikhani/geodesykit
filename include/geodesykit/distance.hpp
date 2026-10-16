// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_DISTANCE_HPP
#define GEODESYKIT_DISTANCE_HPP

#include "coordinate.hpp"
#include "vincenty.hpp"
#include "vec3.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#ifdef GEODESYKIT_USE_GEOGRAPHICLIB
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>
#endif

namespace geodesykit {

struct DistanceBearing {
    double distance = 0.0;  // 3D ECEF chord distance, metres
    double azimuth = 0.0;   // local ENU azimuth, degrees clockwise from north
    double elevation = 0.0; // local ENU elevation, degrees
};

inline DistanceBearing geoDistanceBearing(double lat1, double lon1, double alt1,
                                          double lat2, double lon2, double alt2) noexcept
{
    const double qnan = std::numeric_limits<double>::quiet_NaN();
    const Vec3 p1 = geodetic2ecef(lat1, lon1, alt1);
    const Vec3 p2 = geodetic2ecef(lat2, lon2, alt2);
    const Vec3 d = sub(p2, p1);
    DistanceBearing out;
    if (!std::isfinite(d.x) || !std::isfinite(d.y) || !std::isfinite(d.z)) {
        out.distance = out.azimuth = out.elevation = qnan;
        return out;
    }
    out.distance = norm(d);
    if (out.distance < 1e-9) return out;

    Vec3 east, north, up;
    enuAxes(lat1, lon1, east, north, up);
    const double e = dot(d, east), n = dot(d, north), u = dot(d, up);
    out.azimuth = normalizeAzimuth(std::atan2(e, n) * RAD2DEG);
    out.elevation = std::atan2(u, std::hypot(e, n)) * RAD2DEG;
    return out;
}

/// Ellipsoidal geodesic inverse. Uses GeographicLib when enabled; otherwise
/// Vincenty is attempted first and a clamped spherical fallback is returned
/// for non-convergent cases. `converged=false` identifies that approximation.
inline GeodesicResult geodesicInverse(double lat1, double lon1,
                                      double lat2, double lon2) noexcept
{
    if (!std::isfinite(lat1) || !std::isfinite(lon1) ||
        !std::isfinite(lat2) || !std::isfinite(lon2) ||
        lat1 < -90.0 || lat1 > 90.0 || lat2 < -90.0 || lat2 > 90.0) {
        return geodesicFailure();
    }
#ifdef GEODESYKIT_USE_GEOGRAPHICLIB
    using namespace GeographicLib;
    static const Geodesic geod(Constants::WGS84_a(), Constants::WGS84_f());
    double s12 = 0.0, azi1 = 0.0, azi2 = 0.0;
    geod.Inverse(lat1, lon1, lat2, lon2, s12, azi1, azi2);
    return GeodesicResult{s12, normalizeAzimuth(azi1), normalizeAzimuth(azi2), true};
#else
    GeodesicResult result = vincentyInverse(lat1, lon1, lat2, lon2);
    if (result.converged) return result;

    constexpr double earthMeanRadius = 6371008.8;
    const double phi1 = lat1 * DEG2RAD, phi2 = lat2 * DEG2RAD;
    const double dphi = (lat2 - lat1) * DEG2RAD;
    const double dlam = std::remainder(lon2 - lon1, 360.0) * DEG2RAD;
    const double sdphi = std::sin(dphi * 0.5);
    const double sdlam = std::sin(dlam * 0.5);
    const double hav = sdphi * sdphi + std::cos(phi1) * std::cos(phi2) * sdlam * sdlam;
    const double safeHav = std::clamp(hav, 0.0, 1.0);
    const double central = 2.0 * std::atan2(std::sqrt(safeHav), std::sqrt(1.0 - safeHav));
    const double azi1 = std::atan2(std::cos(phi2) * std::sin(dlam),
        std::cos(phi1) * std::sin(phi2) - std::sin(phi1) * std::cos(phi2) * std::cos(dlam));
    const double azi2 = std::atan2(std::cos(phi1) * std::sin(dlam),
        -std::sin(phi1) * std::cos(phi2) + std::cos(phi1) * std::sin(phi2) * std::cos(dlam));
    return GeodesicResult{earthMeanRadius * central,
        normalizeAzimuth(azi1 * RAD2DEG), normalizeAzimuth(azi2 * RAD2DEG), false};
#endif
}

} // namespace geodesykit
#endif // GEODESYKIT_DISTANCE_HPP
