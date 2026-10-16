// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_CONSTANTS_HPP
#define GEODESYKIT_CONSTANTS_HPP
namespace geodesykit {
constexpr double WGS84_A = 6378137.0;
constexpr double WGS84_F = 1.0 / 298.257223563;
constexpr double WGS84_E2 = 6.6943799901413165e-3;
constexpr double WGS84_B = WGS84_A * (1.0 - WGS84_F);
constexpr double WGS84_EP2 = (WGS84_A * WGS84_A - WGS84_B * WGS84_B) / (WGS84_B * WGS84_B);
constexpr double DEG2RAD = 3.14159265358979323846 / 180.0;
constexpr double RAD2DEG = 180.0 / 3.14159265358979323846;
constexpr double PI = 3.14159265358979323846;
}
#endif
