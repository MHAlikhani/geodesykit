// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Mohammad Hossein Alikhani
#ifndef GEODESYKIT_TEST_COMMON_HPP
#define GEODESYKIT_TEST_COMMON_HPP
#include <cmath>
#include <cstdio>
#include <cstdlib>
#define REQUIRE(cond) do { \
    if (!(cond)) { \
        std::fprintf(stderr, "REQUIRE failed: %s\n  at %s:%d\n", #cond, __FILE__, __LINE__); \
        std::exit(1); \
    } \
} while (0)
#define REQUIRE_NEAR(a, b, tol) do { \
    const double _a = (a); const double _b = (b); const double _t = (tol); \
    if (!std::isfinite(_a) || !std::isfinite(_b) || !std::isfinite(_t) || _t < 0.0 || std::fabs(_a - _b) > _t) { \
        std::fprintf(stderr, "REQUIRE_NEAR failed: |%.17g - %.17g| = %.17g > %.17g\n  at %s:%d\n", \
                     _a, _b, std::fabs(_a - _b), _t, __FILE__, __LINE__); \
        std::exit(1); \
    } \
} while (0)
#endif // GEODESYKIT_TEST_COMMON_HPP
