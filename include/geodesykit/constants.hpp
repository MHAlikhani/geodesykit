// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
//
// GeodesyKit — A Modern C++ Geodesy Toolkit
// https://github.com/MHAlikhani/geodesykit

#ifndef GEODESYKIT_CONSTANTS_HPP
#define GEODESYKIT_CONSTANTS_HPP

namespace geodesykit {

/// WGS84 semi-major axis (meters)
constexpr double WGS84_A = 6378137.0;

/// WGS84 flattening
constexpr double WGS84_F = 1.0 / 298.257223563;

/// WGS84 first eccentricity squared
constexpr double WGS84_E2 = 6.6943799901413165e-3;

/// WGS84 semi-minor axis (meters)
constexpr double WGS84_B = WGS84_A * (1.0 - WGS84_F);

/// WGS84 second eccentricity squared e'^2 = (a^2 - b^2) / b^2
constexpr double WGS84_EP2 = (WGS84_A * WGS84_A - WGS84_B * WGS84_B) / (WGS84_B * WGS84_B);

/// Degrees -> Radians
constexpr double DEG2RAD = 3.14159265358979323846 / 180.0;

/// Radians -> Degrees
constexpr double RAD2DEG = 180.0 / 3.14159265358979323846;

/// pi
constexpr double PI = 3.14159265358979323846;

} // namespace geodesykit

#endif // GEODESYKIT_CONSTANTS_HPP