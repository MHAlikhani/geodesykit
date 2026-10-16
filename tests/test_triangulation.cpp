// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#include <geodesykit/geodesykit.hpp>
#include "test_common.hpp"
#include <utility>

using namespace geodesykit;

int main()
{
    const double lat_A = 50.913172, lon_A = 8.005806, h_A = 550.0;
    const double lat_B = 50.935689, lon_B = 8.013476, h_B = 400.0;

    const double lat_T = 50.920000, lon_T = 8.010000, h_T = 450.0;

    Vec3 T = geodetic2ecef(lat_T, lon_T, h_T);

    auto bearing = [&](double lat, double lon, double h) {
        Vec3 P = geodetic2ecef(lat, lon, h);
        Vec3 d = sub(T, P);
        Vec3 E, N, U;
        enuAxes(lat, lon, E, N, U);
        double e = dot(d, E), n = dot(d, N), u = dot(d, U);
        double az = std::atan2(e, n) * RAD2DEG;
        if (az < 0) az += 360.0;
        double el = std::atan2(u, std::hypot(e, n)) * RAD2DEG;
        return std::pair<double,double>{az, el};
    };

    auto [azA, elA] = bearing(lat_A, lon_A, h_A);
    auto [azB, elB] = bearing(lat_B, lon_B, h_B);

    // ---- Test 1: basic triangulation ----
    {
        auto res = triangulate3D(lat_A, lon_A, h_A, azA, elA,
                                 lat_B, lon_B, h_B, azB, elB,
                                 1.0, 1.0, 1e-6);

        REQUIRE(!res.degenerate);
        REQUIRE_NEAR(res.lat, lat_T, 1e-5);
        REQUIRE_NEAR(res.lon, lon_T, 1e-5);
        REQUIRE_NEAR(res.alt, h_T,   1e-2);

        std::printf("Test 1 (Kalteiche): lat=%.8f, lon=%.8f, alt=%.3f m\n",
                    res.lat, res.lon, res.alt);
        std::printf("  Residual: %.3e m\n", res.residual);
    }

    // ---- Test 2: weighting ----
    {
        auto res_w = triangulate3D(lat_A, lon_A, h_A, azA, elA,
                                   lat_B, lon_B, h_B, azB, elB,
                                   100.0, 1.0, 1e-6);
        double dA = std::hypot(res_w.lat - lat_A, res_w.lon - lon_A);
        double dB = std::hypot(res_w.lat - lat_B, res_w.lon - lon_B);
        REQUIRE(dA < dB);
        std::printf("Test 2 (weighted): dA=%.6f deg, dB=%.6f deg -> OK\n", dA, dB);
    }

    // ---- Test 3: degenerate case ----
    {
        auto res_deg = triangulate3D(lat_A, lon_A, h_A, 45.0, 10.0,
                                     lat_A, lon_A, h_A, 45.0001, 10.0,
                                     1.0, 1.0, 1e-6);
        REQUIRE(res_deg.degenerate || res_deg.condition > 1e6);
        std::printf("Test 3 (degenerate): degenerate=%s, cond=%.3e\n",
                    res_deg.degenerate ? "yes" : "no", res_deg.condition);
    }

    // ---- Test 4: invalid input ----
    {
        auto res_bad = triangulate3D(lat_A, lon_A, h_A, azA, elA,
                                     lat_B, lon_B, h_B, azB, elB,
                                     -1.0, 1.0, 1e-6);
        REQUIRE(res_bad.degenerate);
        std::printf("Test 4 (invalid weight): correctly rejected\n");
    }

    std::printf("\nAll triangulation tests passed.\n");
    return 0;
}