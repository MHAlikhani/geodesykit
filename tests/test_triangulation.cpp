// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#include <geodesykit/geodesykit.hpp>
#include "test_common.hpp"
#include <cmath>
#include <cstdio>
#include <limits>
#include <utility>
using namespace geodesykit;

int main()
{
    const double latA = 50.913172, lonA = 8.005806, hA = 550.0;
    const double latB = 50.935689, lonB = 8.013476, hB = 400.0;
    const double latT = 50.920000, lonT = 8.010000, hT = 450.0;
    const Vec3 target = geodetic2ecef(latT, lonT, hT);
    auto bearing = [&](double lat, double lon, double h) {
        const Vec3 p = geodetic2ecef(lat, lon, h);
        const Vec3 d = sub(target, p);
        Vec3 e, n, u;
        enuAxes(lat, lon, e, n, u);
        const double east = dot(d, e), north = dot(d, n), up = dot(d, u);
        return std::pair<double,double>{normalizeAzimuth(std::atan2(east, north) * RAD2DEG),
                                        std::atan2(up, std::hypot(east, north)) * RAD2DEG};
    };
    const auto [azA, elA] = bearing(latA, lonA, hA);
    const auto [azB, elB] = bearing(latB, lonB, hB);
    const auto result = triangulate3D(latA, lonA, hA, azA, elA,
                                      latB, lonB, hB, azB, elB, 1.0, 1.0, 1e-12);
    REQUIRE(!result.degenerate);
    REQUIRE_NEAR(result.lat, latT, 1e-5);
    REQUIRE_NEAR(result.lon, lonT, 1e-5);
    REQUIRE_NEAR(result.alt, hT, 0.1);
    REQUIRE(result.residual < 0.1);

    const auto invalidWeight = triangulate3D(latA, lonA, hA, azA, elA,
        latB, lonB, hB, azB, elB, -1.0, 1.0, 1e-6);
    REQUIRE(invalidWeight.degenerate);
    const auto nanWeight = triangulate3D(latA, lonA, hA, azA, elA,
        latB, lonB, hB, azB, elB, std::numeric_limits<double>::quiet_NaN(), 1.0, 1e-6);
    REQUIRE(nanWeight.degenerate);

    const auto parallel = triangulate3D(latA, lonA, hA, 45.0, 10.0,
        latA, lonA, hA, 45.000001, 10.0, 1.0, 1.0, 0.0);
    REQUIRE(parallel.degenerate);
    std::printf("All triangulation tests passed.\n");
    return 0;
}
