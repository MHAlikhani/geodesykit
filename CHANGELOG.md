# Changelog

All notable changes to GeodesyKit will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Python bindings via pybind11
- GeographicLib-backed geodesic direct problem
- Multi-observer triangulation (>2 rays)
- Optional Eigen interop

## [0.1.0] - 2026-10-16

### Added
- Initial public release.
- WGS84 constants and second eccentricity squared (`constants.hpp`).
- 3D vector primitives with optional FMA acceleration (`vec3.hpp`).
- Vermeille ECEF → geodetic conversion, machine precision (`vermeille.hpp`).
- Geodetic ↔ ECEF transforms and ENU axes (`coordinate.hpp`).
- Vincenty inverse geodesic with convergence flag (`vincenty.hpp`).
- Geodesic distance (Karney / Vincenty / Haversine fallback) and 3D bearing (`distance.hpp`).
- Weighted least-squares triangulation with Tikhonov regularization (`triangulation.hpp`).
- CMake build system with install/export support and CPack.
- Test suite with REQUIRE/REQUIRE_NEAR macros, using NRW (Germany) reference coordinates.
- Example program demonstrating typical usage and error handling.

[Unreleased]: https://github.com/MHAlikhani/geodesykit/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/MHAlikhani/geodesykit/releases/tag/v0.1.0