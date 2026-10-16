// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#include <geodesykit/geodesykit.hpp>
#include <cstdio>

int main()
{
    using namespace geodesykit;

    // ---- NRW locations ----
    // Siegen (your city)
    const double lat_Siegen = 50.8748, lon_Siegen = 8.0243;
    // Kölner Dom (Cologne Cathedral)
    const double lat_Dom = 50.94131, lon_Dom = 6.95811;
    // Zeche Zollverein (Essen, UNESCO site)
    const double lat_Zollverein = 51.4865, lon_Zollverein = 7.04408;

    // 1) Geodesic distance: Siegen → Kölner Dom
    auto g1 = geodesicInverse(lat_Siegen, lon_Siegen, lat_Dom, lon_Dom);
    std::printf("Siegen -> Koelner Dom: %.3f km\n", g1.distance / 1000.0);
    std::printf("  Forward azimuth: %.6f deg\n", g1.azimuth1);
    std::printf("  Converged: %s\n", g1.converged ? "yes" : "no");

    // 2) Siegen → Zeche Zollverein
    auto g2 = geodesicInverse(lat_Siegen, lon_Siegen,
                              lat_Zollverein, lon_Zollverein);
    std::printf("Siegen -> Zeche Zollverein: %.3f km\n", g2.distance / 1000.0);

    // 3) 3D distance & bearing (Siegen → Kölner Dom, with altitudes)
    auto db = geoDistanceBearing(lat_Siegen, lon_Siegen, 300.0,   // ~300 m
                                 lat_Dom, lon_Dom, 62.0);          // ~62 m
    std::printf("3D distance Siegen -> Dom: %.3f m\n", db.distance);
    std::printf("  Azimuth: %.3f deg, Elevation: %.3f deg\n",
                db.azimuth, db.elevation);

    // 4) Triangulation example: two NRW observers
    // Station A: near Kalteiche (Siegen area)
    const double lat_A = 50.913172, lon_A = 8.005806, h_A = 550.0;
    // Station B: near Kreuztal
    const double lat_B = 50.935689, lon_B = 8.013476, h_B = 400.0;

    auto tri = triangulate3D(
        lat_A, lon_A, h_A,  45.0, 10.0,   // from A: az 45°, el 10°
        lat_B, lon_B, h_B, 225.0,  8.0,   // from B: az 225°, el 8°
        1.0, 1.0, 1e-6);

    std::printf("\nTriangulated point:\n");
    std::printf("  lat=%.8f deg, lon=%.8f deg, alt=%.3f m\n",
                tri.lat, tri.lon, tri.alt);
    std::printf("  Residual: %.3f m, Condition: %.3e\n",
                tri.residual, tri.condition);
    std::printf("  Degenerate: %s\n", tri.degenerate ? "yes" : "no");

    return 0;
}