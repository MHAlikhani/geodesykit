// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_TRIANGULATION_HPP
#define GEODESYKIT_TRIANGULATION_HPP

#include "coordinate.hpp"
#include "vec3.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace geodesykit {

struct TriangulationResult {
    double lat = 0.0;
    double lon = 0.0;
    double alt = 0.0;
    double residual = 0.0;  // RMS perpendicular distance to the two observed lines, metres
    double range1 = 0.0;    // signed projection along ray 1, metres
    double range2 = 0.0;    // signed projection along ray 2, metres
    double condition = 0.0; // pivot-ratio diagnostic; not a formal condition number
    bool degenerate = false;
};

namespace detail {
inline bool finiteVec(const Vec3& v) noexcept
{
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}

inline bool solve3x3(double m[3][3], double rhs[3], double x[3], double& pivotRatio) noexcept
{
    double largest = 0.0, smallestPivot = std::numeric_limits<double>::infinity();
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) largest = std::max(largest, std::fabs(m[r][c]));
    if (!(largest > 0.0) || !std::isfinite(largest)) return false;

    for (int col = 0; col < 3; ++col) {
        int pivot = col;
        for (int r = col + 1; r < 3; ++r)
            if (std::fabs(m[r][col]) > std::fabs(m[pivot][col])) pivot = r;
        const double pv = std::fabs(m[pivot][col]);
        if (!std::isfinite(pv) || pv <= largest * 1e-12) return false;
        smallestPivot = std::min(smallestPivot, pv);
        if (pivot != col) {
            for (int c = col; c < 3; ++c) std::swap(m[pivot][c], m[col][c]);
            std::swap(rhs[pivot], rhs[col]);
        }
        for (int r = col + 1; r < 3; ++r) {
            const double factor = m[r][col] / m[col][col];
            for (int c = col; c < 3; ++c) m[r][c] -= factor * m[col][c];
            rhs[r] -= factor * rhs[col];
        }
    }
    double largestPivot = 0.0;
    for (int i = 0; i < 3; ++i) largestPivot = std::max(largestPivot, std::fabs(m[i][i]));
    pivotRatio = largestPivot / smallestPivot;
    for (int r = 2; r >= 0; --r) {
        double value = rhs[r];
        for (int c = r + 1; c < 3; ++c) value -= m[r][c] * x[c];
        x[r] = value / m[r][r];
        if (!std::isfinite(x[r])) return false;
    }
    return true;
}

inline Vec3 projectPerpendicular(const Vec3& v, const Vec3& unit) noexcept
{
    return sub(v, mul(unit, dot(v, unit)));
}
} // namespace detail

/// Estimate the point minimizing weighted squared perpendicular distances to
/// two bearing lines. Weights are positive relative weights (inverse variances
/// if the perpendicular residual model is appropriate). lambda_reg weakly
/// regularizes displacement from the station midpoint; it is dimensionless.
/// Forward-ray violations are reported as degenerate rather than silently
/// clamping the ranges and returning a point that is not the least-squares fit.
inline TriangulationResult triangulate3D(double lat1, double lon1, double h1,
                                         double az1, double el1,
                                         double lat2, double lon2, double h2,
                                         double az2, double el2,
                                         double weight1 = 1.0,
                                         double weight2 = 1.0,
                                         double lambda_reg = 1e-6) noexcept
{
    const double qnan = std::numeric_limits<double>::quiet_NaN();
    TriangulationResult out;
    const auto fail = [&]() noexcept {
        out.lat = out.lon = out.alt = out.residual = out.range1 = out.range2 = qnan;
        out.degenerate = true;
        return out;
    };
    if (!std::isfinite(lat1) || !std::isfinite(lon1) || !std::isfinite(h1) ||
        !std::isfinite(az1) || !std::isfinite(el1) ||
        !std::isfinite(lat2) || !std::isfinite(lon2) || !std::isfinite(h2) ||
        !std::isfinite(az2) || !std::isfinite(el2) ||
        lat1 < -90.0 || lat1 > 90.0 || lat2 < -90.0 || lat2 > 90.0 ||
        el1 < -90.0 || el1 > 90.0 || el2 < -90.0 || el2 > 90.0 ||
        !std::isfinite(weight1) || !std::isfinite(weight2) ||
        !std::isfinite(lambda_reg) || weight1 <= 0.0 || weight2 <= 0.0 || lambda_reg < 0.0) {
        return fail();
    }

    const Vec3 p1 = geodetic2ecef(lat1, lon1, h1);
    const Vec3 p2 = geodetic2ecef(lat2, lon2, h2);
    if (!detail::finiteVec(p1) || !detail::finiteVec(p2)) return fail();

    Vec3 e1, n1, u1, e2, n2, u2;
    enuAxes(lat1, lon1, e1, n1, u1);
    enuAxes(lat2, lon2, e2, n2, u2);
    const double a1 = std::remainder(az1, 360.0) * DEG2RAD;
    const double a2 = std::remainder(az2, 360.0) * DEG2RAD;
    const double rEl1 = el1 * DEG2RAD, rEl2 = el2 * DEG2RAD;
    const Vec3 d1 = normalize(add(add(mul(e1, std::cos(rEl1) * std::sin(a1)),
                                     mul(n1, std::cos(rEl1) * std::cos(a1))),
                                     mul(u1, std::sin(rEl1))));
    const Vec3 d2 = normalize(add(add(mul(e2, std::cos(rEl2) * std::sin(a2)),
                                     mul(n2, std::cos(rEl2) * std::cos(a2))),
                                     mul(u2, std::sin(rEl2))));
    if (!detail::finiteVec(d1) || !detail::finiteVec(d2) ||
        norm(d1) < 0.99 || norm(d2) < 0.99) return fail();

    const Vec3 anchor = mul(add(p1, p2), 0.5);
    const Vec3 q1 = sub(p1, anchor), q2 = sub(p2, anchor);
    const Vec3 dirs[2] = {d1, d2};
    const Vec3 stations[2] = {q1, q2};
    const double weights[2] = {weight1, weight2};
    double matrix[3][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
    double rhs[3] = {0.0, 0.0, 0.0};

    // Sum w_i (I - d_i d_i^T) X = sum w_i (I - d_i d_i^T) P_i.
    // Work relative to the station midpoint to reduce ECEF cancellation.
    for (int i = 0; i < 2; ++i) {
        const Vec3& d = dirs[i];
        const Vec3 projected = detail::projectPerpendicular(stations[i], d);
        const double dv[3] = {d.x, d.y, d.z};
        const double pv[3] = {projected.x, projected.y, projected.z};
        for (int r = 0; r < 3; ++r) {
            rhs[r] += weights[i] * pv[r];
            for (int c = 0; c < 3; ++c)
                matrix[r][c] += weights[i] * ((r == c ? 1.0 : 0.0) - dv[r] * dv[c]);
        }
    }
    for (int i = 0; i < 3; ++i) {
        matrix[i][i] += lambda_reg;
        // Regularization target is the midpoint (zero in the local frame).
    }

    double delta[3] = {0.0, 0.0, 0.0};
    double pivotRatio = 0.0;
    if (!detail::solve3x3(matrix, rhs, delta, pivotRatio)) return fail();
    out.condition = pivotRatio;
    if (!std::isfinite(pivotRatio) || pivotRatio > 1e12) return fail();

    const Vec3 estimate = add(anchor, Vec3{delta[0], delta[1], delta[2]});
    const Vec3 from1 = sub(estimate, p1), from2 = sub(estimate, p2);
    out.range1 = dot(from1, d1);
    out.range2 = dot(from2, d2);
    if (out.range1 < 0.0 || out.range2 < 0.0) return fail();

    const double r1 = norm(detail::projectPerpendicular(from1, d1));
    const double r2 = norm(detail::projectPerpendicular(from2, d2));
    out.residual = std::sqrt((weight1 * r1 * r1 + weight2 * r2 * r2) /
                             (weight1 + weight2));
    ecef2geodetic(estimate.x, estimate.y, estimate.z, out.lat, out.lon, out.alt);
    if (!std::isfinite(out.lat) || !std::isfinite(out.lon) || !std::isfinite(out.alt) ||
        !std::isfinite(out.residual)) return fail();
    out.degenerate = false;
    return out;
}

} // namespace geodesykit
#endif // GEODESYKIT_TRIANGULATION_HPP
