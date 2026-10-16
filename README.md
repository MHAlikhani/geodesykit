# GeodesyKit

**GeodesyKit** is a lightweight, header-only C++17 toolkit for common geodetic calculations. It provides geodetic/ECEF coordinate conversions, local ENU axes, geodesic inverse calculations, distance and bearing helpers, and 3D bearing-ray triangulation.

> **Status:** This README describes the proposed correction overlay for the repository. The overlay has not been published to the upstream GitHub repository as a release. Validate it on your target compilers and platforms before production use.

## Features

- WGS 84 ellipsoid constants.
- Geodetic latitude/longitude/height to Earth-Centered, Earth-Fixed (ECEF) coordinates.
- ECEF to geodetic coordinate conversion with guarded handling of invalid and difficult inputs.
- East-North-Up (ENU) basis vectors for a local tangent frame.
- Vincenty inverse geodesic calculations with an explicit convergence status.
- Optional GeographicLib-backed geodesic calculations.
- A spherical fallback for non-convergent Vincenty cases when GeographicLib is disabled; fallback results are marked `converged == false` because they are approximate.
- 3D triangulation from two station locations and azimuth/elevation observations, using weighted perpendicular-distance least squares.
- CMake install/export support and optional tests and example build.

## Requirements

- A C++17-compatible compiler.
- CMake 3.16 or newer to configure the project.
- GeographicLib is optional and required only when `GEODESYKIT_USE_GEOGRAPHICLIB=ON`.

## Build

Clone the repository, then configure and build it:

```sh
git clone https://github.com/MHAlikhani/geodesykit.git
cd geodesykit
cmake -S . -B build \
  -DGEODESYKIT_BUILD_TESTS=ON \
  -DGEODESYKIT_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

To use GeographicLib for geodesic calculations, install its development package and configure with:

```sh
cmake -S . -B build-geographiclib \
  -DGEODESYKIT_USE_GEOGRAPHICLIB=ON \
  -DGEODESYKIT_BUILD_TESTS=ON
cmake --build build-geographiclib --parallel
ctest --test-dir build-geographiclib --output-on-failure
```

The GeographicLib package must provide the CMake target `GeographicLib::GeographicLib`. If your platform's package exports a differently named target, adjust the CMake integration for that package.

## Use as a header-only library

The public headers are under `include/geodesykit/`. Include the umbrella header to access the library's main API:

```cpp
#include <geodesykit/geodesykit.hpp>

int main()
{
    // WGS 84 geodetic coordinates: latitude and longitude in degrees,
    // height in metres.
    const geodesykit::Vec3 ecef =
        geodesykit::geodetic2ecef(51.5074, -0.1278, 35.0);

    double lat = 0.0;
    double lon = 0.0;
    double height = 0.0;
    geodesykit::ecef2geodetic(
        ecef.x, ecef.y, ecef.z, lat, lon, height);

    return 0;
}
```

Compile with C++17 and add the `include` directory to your compiler's include paths. With CMake, link against `GeodesyKit::GeodesyKit` when using the project as a subdirectory or installed package.

## Coordinate conventions

| Quantity | Convention |
|---|---|
| Latitude | Geodetic latitude, degrees, range `[-90, 90]` |
| Longitude | Degrees; values are interpreted periodically where supported |
| Height / altitude | Metres above the reference ellipsoid |
| ECEF coordinates | Metres, Earth-Centered Earth-Fixed Cartesian frame |
| Azimuth | Degrees clockwise from local north, normalized to `[0, 360)` in geodesic results |
| Elevation | Degrees relative to the local horizontal plane |
| Distance | Metres |

## Main API

### Coordinate conversion

```cpp
geodesykit::Vec3 geodesykit::geodetic2ecef(
    double latDeg, double lonDeg, double h,
    double a = geodesykit::WGS84_A,
    double e2 = geodesykit::WGS84_E2) noexcept;

void geodesykit::ecef2geodetic(
    double x, double y, double z,
    double& latDeg, double& lonDeg, double& h,
    double a = geodesykit::WGS84_A,
    double e2 = geodesykit::WGS84_E2) noexcept;

void geodesykit::enuAxes(
    double latDeg, double lonDeg,
    geodesykit::Vec3& E,
    geodesykit::Vec3& N,
    geodesykit::Vec3& U) noexcept;
```

Invalid coordinate or ellipsoid inputs are reported using NaN-valued outputs where the API uses numeric return values or output references. At the ECEF origin, geodetic latitude and longitude are undefined; the inverse conversion reports NaNs rather than inventing a unique location. Callers should check outputs for finiteness.

### Geodesic inverse and distance/bearing helpers

`vincentyInverse(lat1, lon1, lat2, lon2, maxIter)` returns a `GeodesicResult` containing distance in metres, forward azimuths at both endpoints, and a `converged` flag. Near-antipodal pairs may not converge under Vincenty's method; always inspect this flag.

`geodesicInverse(lat1, lon1, lat2, lon2)` uses GeographicLib when the build option is enabled. Otherwise, it attempts Vincenty and returns a spherical approximation if Vincenty fails. In the fallback case, `converged` is `false`; this flag must not be ignored when accuracy matters.

`geoDistanceBearing(...)` provides a 3D ECEF chord distance and local ENU azimuth/elevation for two geodetic positions. Its distance is a straight-line chord distance, not the surface geodesic distance.

### 3D triangulation

`triangulate3D(...)` accepts two stations, each represented by latitude, longitude, height, azimuth and elevation, followed by optional positive weights and a non-negative regularization parameter. The solver minimizes weighted squared perpendicular distances to the two observed bearing lines, with a weak regularization term around the station midpoint.

The result contains the estimated geodetic position, weighted RMS perpendicular residual, projected ranges along both bearing directions, a pivot-ratio diagnostic, and a `degenerate` status. `condition` is a pivot-ratio diagnostic, **not** a formal matrix condition number. Invalid input, failed solution, poor geometry, or a solution behind either forward ray is reported as degenerate; callers must check `degenerate` before using the estimated coordinates.

Weights are relative weights. They can be interpreted as inverse variances only when that interpretation is appropriate for the perpendicular-distance residual model. They are not automatically converted from angular measurement uncertainties.

## Error handling and numerical limitations

- Validate results for finiteness and inspect each API's status flags.
- Vincenty's inverse method is not globally convergent, especially for nearly antipodal points. Use GeographicLib when robust ellipsoidal geodesics are required.
- The no-GeographicLib fallback is spherical and approximate; it is not equivalent to an ellipsoidal geodesic solution.
- ECEF points near the ellipsoid's evolute or deep inside the ellipsoid can have ambiguous or numerically difficult geodetic representations. A failed conversion should be treated as a failure, not as a meaningful coordinate.
- Two-ray triangulation can be poorly determined when the observation geometry is close to parallel. Check `degenerate`, the residual, ranges and pivot-ratio diagnostic, and validate results against the application's physical constraints.
- Numerical tolerances depend on input scale, geometry, compiler and platform. Do not infer universal nanometre-level accuracy from successful round-trip tests alone.

## Tests

The CMake option `GEODESYKIT_BUILD_TESTS` builds independent test executables for coordinate conversion, distance/geodesic calculations, and triangulation. Run them with:

```sh
ctest --test-dir build --output-on-failure
```

For production qualification, also test on the actual target toolchains and compare representative results with an independent trusted implementation such as GeographicLib. Include antipodal and near-antipodal pairs, poles, antimeridian crossings, negative and high altitudes, invalid/non-finite inputs, and nearly parallel triangulation rays.

## Install and consume from CMake

```sh
cmake --install build --prefix ./install
```

A consuming CMake project can then use `find_package(GeodesyKit CONFIG REQUIRED)` and link `GeodesyKit::GeodesyKit`, provided the install prefix is in `CMAKE_PREFIX_PATH`:

```cmake
find_package(GeodesyKit CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE GeodesyKit::GeodesyKit)
```

When GeodesyKit was built with GeographicLib enabled, the package configuration also discovers GeographicLib for consumers.

## License

This project is distributed under the MIT License. See [`LICENSE`](LICENSE) for the complete license text.
