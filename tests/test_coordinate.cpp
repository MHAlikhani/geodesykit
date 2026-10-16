// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#include <geodesykit/geodesykit.hpp>
#include "test_common.hpp"

using namespace geodesykit;

int main()
{
    // ---- Test 1: Siegen round-trip ----
    {
        const double lat0 = 50.8748, lon0 = 8.0243, h0 = 300.0;
        Vec3 e = geodetic2ecef(lat0, lon0, h0);

        double lat, lon, h;
        ecef2geodetic(e.x, e.y, e.z, lat, lon, h);

        REQUIRE_NEAR(lat, lat0, 1e-10);
        REQUIRE_NEAR(lon, lon0, 1e-10);
        REQUIRE_NEAR(h,   h0,   1e-6);
        std::printf("Siegen round-trip: OK (lat=%.10f, lon=%.10f, h=%.6f)\n",
                    lat, lon, h);
    }

    // ---- Test 2: Koelner Dom round-trip ----
    {
        const double lat0 = 50.94131, lon0 = 6.95811, h0 = 62.0;
        Vec3 e = geodetic2ecef(lat0, lon0, h0);

        double lat, lon, h;
        ecef2geodetic(e.x, e.y, e.z, lat, lon, h);

        REQUIRE_NEAR(lat, lat0, 1e-10);
        REQUIRE_NEAR(lon, lon0, 1e-10);
        REQUIRE_NEAR(h,   h0,   1e-6);
        std::printf("Koelner Dom round-trip: OK\n");
    }

    // ---- Test 3: Zeche Zollverein round-trip ----
    {
        const double lat0 = 51.4865, lon0 = 7.04408, h0 = 100.0;
        Vec3 e = geodetic2ecef(lat0, lon0, h0);

        double lat, lon, h;
        ecef2geodetic(e.x, e.y, e.z, lat, lon, h);

        REQUIRE_NEAR(lat, lat0, 1e-10);
        REQUIRE_NEAR(lon, lon0, 1e-10);
        REQUIRE_NEAR(h,   h0,   1e-6);
        std::printf("Zeche Zollverein round-trip: OK\n");
    }

    // ---- Test 4: Equator / Prime Meridian (reference) ----
    {
        Vec3 e = geodetic2ecef(0.0, 0.0, 0.0);
        REQUIRE_NEAR(e.x, WGS84_A, 1e-9);
        REQUIRE_NEAR(e.y, 0.0,     1e-9);
        REQUIRE_NEAR(e.z, 0.0,     1e-9);
        std::printf("Equator/Prime Meridian: OK\n");
    }

    // ---- Test 5: North Pole ----
    {
        Vec3 e = geodetic2ecef(90.0, 0.0, 0.0);
        REQUIRE_NEAR(e.z, WGS84_B, 1e-6);
        std::printf("North Pole: OK (z=%.3f m)\n", e.z);
    }

    // ---- Test 6: Cologne Cathedral ECEF radius sanity ----
    {
        Vec3 e = geodetic2ecef(50.94131, 6.95811, 62.0);
        const double r = std::hypot(e.x, e.y, e.z);
        REQUIRE(r > 6360000.0 && r < 6370000.0);
        std::printf("Koelner Dom ECEF radius: %.3f m\n", r);
    }

    std::printf("\nAll coordinate tests passed.\n");
    return 0;
}