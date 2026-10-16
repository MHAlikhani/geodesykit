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
    const auto local = geodesicInverse(50.8748, 8.0243, 50.94131, 6.95811);
    REQUIRE(local.converged);
    REQUIRE(local.distance > 60000.0 && local.distance < 90000.0);

    const auto quarter = geodesicInverse(0.0, 0.0, 0.0, 90.0);
#ifdef GEODESYKIT_USE_GEOGRAPHICLIB
    REQUIRE(std::fabs(quarter.distance - 10018754.17) < 1.0);
#else
    REQUIRE(std::fabs(quarter.distance - 10018754.17) < 100.0);
#endif

    const auto same = geodesicInverse(12.0, 34.0, 12.0, 394.0);
    REQUIRE(same.converged);
    REQUIRE_NEAR(same.distance, 0.0, 1e-12);

    // Regression: exact antipodes must never be reported as zero distance.
    const auto antipodalDirect = vincentyInverse(0.0, 0.0, 0.0, 180.0);
    REQUIRE(!antipodalDirect.converged);
    const auto antipodal = geodesicInverse(0.0, 0.0, 0.0, 180.0);
    REQUIRE(!antipodal.converged); // spherical fallback is approximate
    REQUIRE(antipodal.distance > 19'000'000.0 && antipodal.distance < 21'000'000.0);

    const auto nearAnti = geodesicInverse(10.0, 20.0, -10.0001, 199.9999);
    REQUIRE(std::isfinite(nearAnti.distance));
    REQUIRE(nearAnti.distance > 19'000'000.0);

    const auto invalid = geodesicInverse(91.0, 0.0, 0.0, 0.0);
    REQUIRE(!invalid.converged);
    REQUIRE(!std::isfinite(invalid.distance));

    std::printf("All distance tests passed.\n");
    return 0;
}
