// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani

#ifndef GEODESYKIT_TRIANGULATION_HPP
#define GEODESYKIT_TRIANGULATION_HPP

#include "coordinate.hpp"
#include "vermeille.hpp"
#include "vec3.hpp"
#include <algorithm>
#include <cmath>

namespace geodesykit {

struct TriangulationResult {
    double lat        = 0.0;
    double lon        = 0.0;
    double alt        = 0.0;
    double residual   = 0.0;
    double range1     = 0.0;
    double range2     = 0.0;
    double condition  = 0.0;
    bool   degenerate = false;
};

/// Weighted least-squares triangulation with Tikhonov regularization.
///
/// @param weight1      inverse variance of first observation
/// @param weight2      inverse variance of second observation
/// @param lambda_reg   regularization parameter (1e-8 .. 1e-4)
inline TriangulationResult triangulate3D(double lat1, double lon1, double h1,
                                         double az1, double el1,
                                         double lat2, double lon2, double h2,
                                         double az2, double el2,
                                         double weight1 = 1.0,
                                         double weight2 = 1.0,
                                         double lambda_reg = 1e-6) noexcept
{
    const Vec3 P1 = geodetic2ecef(lat1, lon1, h1);
    const Vec3 P2 = geodetic2ecef(lat2, lon2, h2);

    Vec3 E1, N1, U1, E2, N2, U2;
    enuAxes(lat1, lon1, E1, N1, U1);
    enuAxes(lat2, lon2, E2, N2, U2);

    const double a1 = az1 * DEG2RAD, e1 = el1 * DEG2RAD;
    const double a2 = az2 * DEG2RAD, e2r = el2 * DEG2RAD;

    const double ce1 = std::cos(e1),  se1 = std::sin(e1);
    const double ca1 = std::cos(a1),  sa1 = std::sin(a1);
    const double ce2 = std::cos(e2r), se2 = std::sin(e2r);
    const double ca2 = std::cos(a2),  sa2 = std::sin(a2);

    const Vec3 u1_enu{ce1 * sa1, ce1 * ca1, se1};
    const Vec3 u2_enu{ce2 * sa2, ce2 * ca2, se2};

    const Vec3 u1 = normalize(add(add(mul(E1, u1_enu.x), mul(N1, u1_enu.y)),
                                   mul(U1, u1_enu.z)));
    const Vec3 u2 = normalize(add(add(mul(E2, u2_enu.x), mul(N2, u2_enu.y)),
                                   mul(U2, u2_enu.z)));

    TriangulationResult res;

    if (norm(u1) < 0.5 || norm(u2) < 0.5) {
        res.degenerate = true;
        return res;
    }

    const Vec3 b = sub(P2, P1);

    const double A11 =  dot(u1, u1);
    const double A12 = -dot(u1, u2);
    const double A21 = -dot(u2, u1);
    const double A22 =  dot(u2, u2);

    const double b1 =  dot(u1, b);
    const double b2 = -dot(u2, b);

    const double M11 = weight1 * A11 + lambda_reg;
    const double M12 = weight1 * A12;
    const double M21 = weight2 * A21;
    const double M22 = weight2 * A22 + lambda_reg;

    const double r1 = weight1 * b1;
    const double r2 = weight2 * b2;

    const double det = M11 * M22 - M12 * M21;
    res.condition = std::fabs(M11 * M22 / (det + 1e-300));

    if (std::fabs(det) < 1e-15) {
        res.degenerate = true;
        res.range1 = std::max(0.0, -dot(u1, b));
        res.range2 = 0.0;
    } else {
        double t1 = (r1 * M22 - M12 * r2) / det;
        double t2 = (M11 * r2 - M21 * r1) / det;

        if (t1 < 0.0 || t2 < 0.0) {
            res.degenerate = true;
            t1 = std::max(0.0, t1);
            t2 = std::max(0.0, t2);
        }

        res.range1 = t1;
        res.range2 = t2;
    }

    const Vec3 Q1 = add(P1, mul(u1, res.range1));
    const Vec3 Q2 = add(P2, mul(u2, res.range2));
    const Vec3 Mp = mul(add(Q1, Q2), 0.5);

    res.residual = norm(sub(Q1, Q2));
    ecef2geodetic(Mp.x, Mp.y, Mp.z, res.lat, res.lon, res.alt);
    return res;
}

} // namespace geodesykit

#endif // GEODESYKIT_TRIANGULATION_HPP