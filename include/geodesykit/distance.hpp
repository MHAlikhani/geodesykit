// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#ifndef GEODESYKIT_DISTANCE_HPP
#define GEODESYKIT_DISTANCE_HPP

#include "coordinate.hpp"
#include "vincenty.hpp"
#include "vec3.hpp"
#include <cmath>

#ifdef GEODESYKIT_USE_GEOGRAPHICLIB
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>
#endif

namespace geodesykit {

struct DistanceBearing {
    double distance  = 0.0;   // meters (3D chord)
    double azimuth   = 0.0;   // degrees
    double elevation = 0.0;   // degrees
};

/// 3D distance and bearing between two geodetic points (ENU-based).
inline DistanceBearing geoDistanceBearing(double lat1, double lon1, double alt1,
                                          double lat2, double lon2, double alt2) noexcept
{
    const Vec3 P1 = geodetic2ecef(lat1, lon1, alt1);
    const Vec3 P2 = geodetic2ecef(lat2, lon2, alt2);
    const Vec3 d  = sub(P2, P1);

    DistanceBearing r;
    r.distance = norm(d);

    Vec3 E, N, U;
    enuAxes(lat1, lon1, E, N, U);

    const double east  = dot(d, E);
    const double north = dot(d, N);
    const double up    = dot(d, U);

    double az = std::atan2(east, north) * RAD2DEG;
    if (az < 0.0) az += 360.0;
    r.azimuth = az;

    const double horiz = std::hypot(east, north);
    r.elevation = std::atan2(up, horiz) * RAD2DEG;
    return r;
}

/// Geodesic distance along WGS84 ellipsoid.
/// Uses GeographicLib (Karney) if GEODESYKIT_USE_GEOGRAPHICLIB is defined;
/// otherwise falls back to Vincenty.
inline GeodesicResult geodesicInverse(double lat1, double lon1,
                                      double lat2, double lon2) noexcept
{
#ifdef GEODESYKIT_USE_GEOGRAPHICLIB
    using namespace GeographicLib;
    static const Geodesic geod(Constants::WGS84_a(), Constants::WGS84_f());
    double s12, azi1, azi2;
    geod.Inverse(lat1, lon1, lat2, lon2, s12, azi1, azi2);
    return GeodesicResult{s12, azi1, azi2, true};
#else
    return vincentyInverse(lat1, lon1, lat2, lon2);
#endif
}

} // namespace geodesykit

#endif // GEODESYKIT_DISTANCE_HPP