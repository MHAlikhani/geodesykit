// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
//
// Vincenty inverse formula — fallback for environments without
// GeographicLib. Accuracy ~0.5 mm on WGS84, but may fail to converge
// near antipodal points.

#ifndef GEODESYKIT_VINCENTY_HPP
#define GEODESYKIT_VINCENTY_HPP

#include "constants.hpp"
#include <cmath>

namespace geodesykit {

struct GeodesicResult {
    double distance  = 0.0;   // meters
    double azimuth1  = 0.0;   // degrees, forward azimuth at point 1
    double azimuth2  = 0.0;   // degrees, forward azimuth at point 2
    bool   converged = true;
};

inline GeodesicResult vincentyInverse(double lat1, double lon1,
                                      double lat2, double lon2,
                                      int maxIter = 100) noexcept
{
    const double a = WGS84_A;
    const double f = WGS84_F;
    const double b = a * (1.0 - f);

    const double L  = (lon2 - lon1) * DEG2RAD;
    const double U1 = std::atan((1.0 - f) * std::tan(lat1 * DEG2RAD));
    const double U2 = std::atan((1.0 - f) * std::tan(lat2 * DEG2RAD));

    const double sU1 = std::sin(U1), cU1 = std::cos(U1);
    const double sU2 = std::sin(U2), cU2 = std::cos(U2);

    double lambda = L;
    double lambdaPrev = 0.0;
    double sigma = 0.0, sSigma = 0.0, cSigma = 0.0;
    double sAlpha = 0.0, c2Alpha = 0.0, c2SigmaM = 0.0;
    bool converged = false;

    for (int i = 0; i < maxIter; ++i) {
        const double sLambda = std::sin(lambda), cLambda = std::cos(lambda);
        const double t1 = cU2 * sLambda;
        const double t2 = cU1 * sU2 - sU1 * cU2 * cLambda;
        const double sinSqSigma = t1 * t1 + t2 * t2;

        sSigma = std::sqrt(sinSqSigma);
        if (sSigma < 1e-15) {
            return GeodesicResult{0.0, 0.0, 0.0, true};
        }

        cSigma   = cU1 * cU2 + sU1 * sU2 * cLambda;
        sigma    = std::atan2(sSigma, cSigma);
        sAlpha   = cU1 * cU2 * sLambda / sSigma;
        c2Alpha  = 1.0 - sAlpha * sAlpha;
        c2SigmaM = (c2Alpha > 1e-15)
                 ? cSigma - 2.0 * sU1 * sU2 / c2Alpha
                 : 0.0;

        const double C = f / 16.0 * c2Alpha *
                         (4.0 + f * (4.0 - 3.0 * c2Alpha));
        lambdaPrev = lambda;
        lambda = L + (1.0 - C) * f * sAlpha *
                 (sigma + C * sSigma *
                  (c2SigmaM + C * cSigma *
                   (-1.0 + 2.0 * c2SigmaM * c2SigmaM)));

        if (std::fabs(lambda - lambdaPrev) < 1e-12) {
            converged = true;
            break;
        }
    }

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

    const double alpha1 = std::atan2(
        cU2 * std::sin(lambda),
        cU1 * sU2 - sU1 * cU2 * std::cos(lambda));

    const double alpha2 = std::atan2(
        cU1 * std::sin(lambda),
        -sU1 * cU2 + cU1 * sU2 * std::cos(lambda));

    GeodesicResult r;
    r.distance  = s12;
    r.azimuth1  = alpha1 * RAD2DEG;
    r.azimuth2  = alpha2 * RAD2DEG;
    r.converged = converged;
    return r;
}

} // namespace geodesykit

#endif // GEODESYKIT_VINCENTY_HPP