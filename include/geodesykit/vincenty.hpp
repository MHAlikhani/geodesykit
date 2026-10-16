// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_VINCENTY_HPP
#define GEODESYKIT_VINCENTY_HPP

#include "constants.hpp"
#include <cmath>
#include <limits>

namespace geodesykit {

struct GeodesicResult {
    double distance = 0.0;   // metres
    double azimuth1 = 0.0;   // forward azimuth at point 1, degrees [0, 360)
    double azimuth2 = 0.0;   // forward azimuth at point 2, degrees [0, 360)
    bool converged = true;   // false means no reliable ellipsoidal result was obtained
};

inline double normalizeAzimuth(double degrees) noexcept
{
    double out = std::fmod(degrees, 360.0);
    if (out < 0.0) out += 360.0;
    return out;
}

inline GeodesicResult geodesicFailure() noexcept
{
    const double qnan = std::numeric_limits<double>::quiet_NaN();
    return GeodesicResult{qnan, qnan, qnan, false};
}

/// Vincenty's inverse solution on WGS84. Near-antipodal cases can fail to
/// converge; callers should inspect `converged` and use a robust fallback.
inline GeodesicResult vincentyInverse(double lat1, double lon1,
                                      double lat2, double lon2,
                                      int maxIter = 100) noexcept
{
    if (!std::isfinite(lat1) || !std::isfinite(lon1) ||
        !std::isfinite(lat2) || !std::isfinite(lon2) ||
        lat1 < -90.0 || lat1 > 90.0 || lat2 < -90.0 || lat2 > 90.0 ||
        maxIter <= 0) {
        return geodesicFailure();
    }

    const double normalizedLon1 = std::remainder(lon1, 360.0);
    const double normalizedLon2 = std::remainder(lon2, 360.0);
    double Ldeg = std::remainder(normalizedLon2 - normalizedLon1, 360.0);
    if (std::fabs(lat1 - lat2) < 1e-14 && std::fabs(Ldeg) < 1e-14) {
        return GeodesicResult{0.0, 0.0, 0.0, true};
    }

    const double a = WGS84_A;
    const double f = WGS84_F;
    const double b = WGS84_B;
    const double L = Ldeg * DEG2RAD;
    const double U1 = std::atan((1.0 - f) * std::tan(lat1 * DEG2RAD));
    const double U2 = std::atan((1.0 - f) * std::tan(lat2 * DEG2RAD));
    const double sU1 = std::sin(U1), cU1 = std::cos(U1);
    const double sU2 = std::sin(U2), cU2 = std::cos(U2);

    double lambda = L;
    double sigma = 0.0, sSigma = 0.0, cSigma = 0.0;
    double sAlpha = 0.0, c2Alpha = 0.0, c2SigmaM = 0.0;
    bool converged = false;

    for (int i = 0; i < maxIter; ++i) {
        const double sLambda = std::sin(lambda), cLambda = std::cos(lambda);
        const double t1 = cU2 * sLambda;
        const double t2 = cU1 * sU2 - sU1 * cU2 * cLambda;
        sSigma = std::hypot(t1, t2);

        // A vanishing sigma is coincident only if the geodetic coordinates
        // were coincident (handled above). Otherwise this is an antipodal
        // singularity and must not be reported as a zero-length path.
        if (sSigma < 1e-15) return geodesicFailure();

        cSigma = sU1 * sU2 + cU1 * cU2 * cLambda;
        sigma = std::atan2(sSigma, cSigma);
        sAlpha = cU1 * cU2 * sLambda / sSigma;
        c2Alpha = std::max(0.0, 1.0 - sAlpha * sAlpha);
        c2SigmaM = c2Alpha > 1e-15
            ? cSigma - 2.0 * sU1 * sU2 / c2Alpha
            : 0.0;
        const double C = f / 16.0 * c2Alpha *
                         (4.0 + f * (4.0 - 3.0 * c2Alpha));
        const double previous = lambda;
        lambda = L + (1.0 - C) * f * sAlpha *
                 (sigma + C * sSigma *
                  (c2SigmaM + C * cSigma * (-1.0 + 2.0 * c2SigmaM * c2SigmaM)));
        if (!std::isfinite(lambda)) return geodesicFailure();
        if (std::fabs(lambda - previous) < 1e-12) {
            converged = true;
            break;
        }
    }
    if (!converged) return geodesicFailure();

    const double u2 = c2Alpha * (a * a - b * b) / (b * b);
    const double A = 1.0 + u2 / 16384.0 *
        (4096.0 + u2 * (-768.0 + u2 * (320.0 - 175.0 * u2)));
    const double B = u2 / 1024.0 *
        (256.0 + u2 * (-128.0 + u2 * (74.0 - 47.0 * u2)));
    const double deltaSigma = B * sSigma *
        (c2SigmaM + B / 4.0 *
         (cSigma * (-1.0 + 2.0 * c2SigmaM * c2SigmaM) -
          B / 6.0 * c2SigmaM * (-3.0 + 4.0 * sSigma * sSigma) *
          (-3.0 + 4.0 * c2SigmaM * c2SigmaM)));
    const double s12 = b * A * (sigma - deltaSigma);
    const double alpha1 = std::atan2(cU2 * std::sin(lambda),
        cU1 * sU2 - sU1 * cU2 * std::cos(lambda));
    const double alpha2 = std::atan2(cU1 * std::sin(lambda),
        -sU1 * cU2 + cU1 * sU2 * std::cos(lambda));
    if (!std::isfinite(s12) || s12 < 0.0 || !std::isfinite(alpha1) || !std::isfinite(alpha2))
        return geodesicFailure();
    return GeodesicResult{s12, normalizeAzimuth(alpha1 * RAD2DEG),
                          normalizeAzimuth(alpha2 * RAD2DEG), true};
}

} // namespace geodesykit
#endif // GEODESYKIT_VINCENTY_HPP
