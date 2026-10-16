// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#include <geodesykit/geodesykit.hpp>
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace geodesykit;

int main()
{
    // ---- Test 1: Siegen → Koelner Dom ----
    // Expected ~70-75 km
    {
        auto g = geodesicInverse(50.8748, 8.0243,
                                 50.94131, 6.95811);
        assert(g.distance > 60000.0 && g.distance < 90000.0);
        std::printf("Siegen -> Koelner Dom: %.3f km\n", g.distance / 1000.0);
    }

    // ---- Test 2: Siegen → Zeche Zollverein (Essen) ----
    // Expected ~80-100 km
    {
        auto g = geodesicInverse(50.8748, 8.0243,
                                 51.4865, 7.04408);
        assert(g.distance > 70000.0 && g.distance < 120000.0);
        std::printf("Siegen -> Zeche Zollverein: %.3f km\n", g.distance / 1000.0);
    }

    // ---- Test 3: Koelner Dom → Drachenfels (Koenigswinter) ----
    // Expected ~35 km
    {
        auto g = geodesicInverse(50.94131, 6.95811,
                                 50.665269, 7.210238);
        assert(g.distance > 25000.0 && g.distance < 50000.0);
        std::printf("Koelner Dom -> Drachenfels: %.3f km\n", g.distance / 1000.0);
    }

    // ---- Test 4: Hermannsdenkmal (Detmold) → Zeche Zollverein ----
    // Expected ~130-160 km
    {
        auto g = geodesicInverse(51.91165, 8.83948,
                                 51.4865, 7.04408);
        assert(g.distance > 100000.0 && g.distance < 200000.0);
        std::printf("Hermannsdenkmal -> Zeche Zollverein: %.3f km\n",
                    g.distance / 1000.0);
    }

    // ---- Test 5: Equator quarter circumference (reference) ----
    // (0,0) → (0,90) ≈ 10,018,754 m
    {
        auto q = geodesicInverse(0.0, 0.0, 0.0, 90.0);
        assert(std::fabs(q.distance - 10018754.17) < 100.0);
        std::printf("Equator (0 -> 90): %.1f m\n", q.distance);
    }

    // ---- Test 6: Symmetry check ----
    // distance(A→B) == distance(B→A)
    {
        auto ab = geodesicInverse(50.8748, 8.0243, 50.94131, 6.95811);
        auto ba = geodesicInverse(50.94131, 6.95811, 50.8748, 8.0243);
        assert(std::fabs(ab.distance - ba.distance) < 1e-6);
        std::printf("Symmetry check: OK\n");
    }

    std::printf("\nAll distance tests passed.\n");
    return 0;
}