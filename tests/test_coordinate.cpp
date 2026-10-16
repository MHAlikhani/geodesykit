// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#include <geodesykit/geodesykit.hpp>
#include "test_common.hpp"
#include <cmath>
#include <cstdio>
#include <limits>
using namespace geodesykit;

int main()
{
    const double samples[][3] = {
        {50.8748, 8.0243, 300.0}, {50.94131, 6.95811, 62.0},
        {-33.8688, 151.2093, 20.0}, {0.0, 180.0, -50.0},
        {89.999, -179.9, 1200.0}
    };
    for (const auto& p : samples) {
        const Vec3 ecef = geodetic2ecef(p[0], p[1], p[2]);
        REQUIRE(std::isfinite(ecef.x) && std::isfinite(ecef.y) && std::isfinite(ecef.z));
        double lat, lon, h;
        ecef2geodetic(ecef.x, ecef.y, ecef.z, lat, lon, h);
        REQUIRE_NEAR(lat, p[0], 1e-9);
        REQUIRE(std::fabs(std::remainder(lon - p[1], 360.0)) < 1e-9);
        REQUIRE_NEAR(h, p[2], 1e-4);
    }

    const Vec3 equator = geodetic2ecef(0.0, 0.0, 0.0);
    REQUIRE_NEAR(equator.x, WGS84_A, 1e-9);
    REQUIRE_NEAR(equator.y, 0.0, 1e-9);
    REQUIRE_NEAR(equator.z, 0.0, 1e-9);

    const Vec3 south = geodetic2ecef(-90.0, 45.0, 0.0);
    REQUIRE_NEAR(south.z, -WGS84_B, 1e-6);

    // The centre has no unique geodetic latitude/longitude.
    double lat, lon, h;
    ecef2geodetic(0.0, 0.0, 0.0, lat, lon, h);
    REQUIRE(!std::isfinite(lat) && !std::isfinite(lon) && !std::isfinite(h));

    const Vec3 invalid = geodetic2ecef(91.0, 0.0, 0.0);
    REQUIRE(!std::isfinite(invalid.x));
    std::printf("All coordinate tests passed.\n");
    return 0;
}
