# Changelog

All notable changes to GeodesyKit are documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project aims to use [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0]

### Fixed

- Prevented exact antipodal points from being misreported as a zero-distance, successfully converged Vincenty solution.
- Added finite-value and range validation to geodesic and coordinate-conversion inputs.
- Guarded the Haversine fallback against round-off pushing its intermediate value outside the valid `[0, 1]` interval.
- Reworked ECEF-to-geodetic conversion to avoid a zero-divisor path near the ellipsoid's evolute and to report undefined ECEF-origin coordinates as NaNs.
- Corrected the triangulation formulation to solve a weighted 3D perpendicular-distance least-squares problem instead of scaling rows of a two-variable ray equation.
- Made invalid inputs, failed solves, poorly conditioned geometry, and estimates behind either forward ray report a degenerate triangulation result rather than silently returning a clamped estimate.
- Updated the `REQUIRE_NEAR` test helper so NaN and infinity cannot pass a numeric comparison unnoticed.
- Corrected CMake test registration to build each test source as its own executable because each test source defines its own `main` function.
- Added conditional GeographicLib dependency discovery to the installed CMake package configuration.

### Changed

- Documented that the non-GeographicLib fallback is spherical and approximate, and that its result has `converged == false`.
- Clarified that the triangulation `condition` field is a pivot-ratio diagnostic, not a formal matrix condition number.
- Clarified the meanings of the 3D ECEF chord distance, geodesic surface distance, azimuth/elevation, triangulation weights and regularization parameter.
- Expanded tests for antipodal geodesics, invalid inputs, coordinate edge cases and triangulation failure cases.
- Updated project documentation to state numerical limitations and avoid unsupported universal accuracy claims.

### Validation still recommended

- Build and run tests on every supported compiler and operating system.
- Compare geodesic results with GeographicLib over a broad test set, including exact and near antipodes.
- Validate ECEF/geodetic conversion around poles, the antimeridian, the ellipsoid surface, and deep-interior points.
- Run Monte Carlo triangulation tests with controlled angular noise and a range of observation geometries.
- Verify installation and downstream `find_package(GeodesyKit CONFIG REQUIRED)` consumption with and without GeographicLib enabled.

## [0.1.0]

- Initial repository version.
