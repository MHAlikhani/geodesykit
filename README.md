# GeodesyKit

**A Modern C++ Geodesy Toolkit**
*High-precision geodetic coordinate transforms, geodesic distances, and 3D triangulation.*

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)](#header-only-usage)
[![Platform](https://img.shields.io/badge/platform-linux%20%7C%20macos%20%7C%20windows-lightgrey.svg)](#requirements)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.XXXXXXX.svg)](https://doi.org/10.5281/zenodo.XXXXXXX)

GeodesyKit is a lightweight, header-only C++17 library for high-precision
geodetic computations on the WGS84 ellipsoid. It provides accurate
ECEF ↔ geodetic coordinate conversion, geodesic distance and azimuth
computation, and weighted 3D triangulation from bearing observations.

---

## Table of Contents

- [Features](#features)
- [Accuracy](#accuracy)
- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [Installation](#installation)
  - [Header-only Usage](#header-only-usage)
  - [CMake Integration](#cmake-integration)
  - [Optional: GeographicLib](#optional-geographiclib)
- [API Overview](#api-overview)
  - [Coordinate Transforms](#coordinate-transforms)
  - [Geodesic Distance](#geodesic-distance)
  - [3D Triangulation](#3d-triangulation)
- [Examples](#examples)
- [Building & Testing](#building--testing)
- [Project Structure](#project-structure)
- [References](#references)
- [Citation](#citation)
- [Author](#author)
- [License](#license)

---

## Features

- **Vermeille's method** for ECEF → geodetic conversion — machine precision, no iteration.
- **Karney's geodesic algorithm** via [GeographicLib](https://geographiclib.sourceforge.io/) (optional) — ~15 nm accuracy.
- **Vincenty inverse** fallback — ~0.5 mm accuracy, no external dependencies.
- **Spherical (Haversine) fallback** for near-antipodal cases where Vincenty fails to converge.
- **Weighted least-squares 3D triangulation** with Tikhonov regularization.
- **Input validation** for weights, regularization parameter, and degenerate geometry.
- **Header-only**, C++17, zero required dependencies.
- **Fully documented** with Doxygen-friendly comments.
- **Tested** against real-world NRW (Germany) coordinates.

---

## Accuracy

| Function | Method | Accuracy |
|----------|--------|----------|
| `geodetic2ecef` | Direct | ~nm |
| `ecef2geodetic` | Vermeille (2002/2004) | ~nm |
| `geodesicInverse` | Karney (GeographicLib) | ~15 nm |
| `geodesicInverse` | Vincenty (fallback) | ~0.5 mm |
| `geodesicInverse` | Haversine (last resort) | ~0.5% |
| `triangulate3D` | Weighted LSQ + Tikhonov | ~cm |

All values assume IEEE-754 `double` (64-bit) and WGS84 ellipsoid parameters.

---

## Requirements

- **C++17** or later (uses `if constexpr`, structured bindings, `std::hypot` 3-arg, `std::fma`).
- A standard-conforming compiler:
  - GCC ≥ 7
  - Clang ≥ 6
  - MSVC ≥ 19.14 (Visual Studio 2017 15.7)
- **Optional**: [GeographicLib](https://geographiclib.sourceforge.io/) for Karney-level geodesic accuracy.

No external dependencies are required for the core library.

---

## Quick Start

```cpp
#include <geodesykit/geodesykit.hpp>
using namespace geodesykit;

int main()
{
    // Siegen, Germany -> Cologne Cathedral, Germany
    auto g = geodesicInverse(50.8748, 8.0243,    // Siegen
                             50.94131, 6.95811); // Koelner Dom

    // g.distance  ~ 71 km
    // g.azimuth1  ~ 275 deg
    // g.converged = true
}
```

Compile:

```bash
g++ -std=c++17 -I include my_app.cpp -o my_app
```

---

## Installation

### Header-only Usage

Simply copy the `include/geodesykit/` directory into your project and add:

```cpp
#include <geodesykit/geodesykit.hpp>
```

That's it — no build step required.

### CMake Integration

**Option A — `add_subdirectory`** (recommended for monorepos):

```cmake
add_subdirectory(third_party/geodesykit)
target_link_libraries(myapp PRIVATE GeodesyKit::GeodesyKit)
```

**Option B — `find_package`** (after installing):

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build --target install
```

Then in your project:

```cmake
find_package(GeodesyKit REQUIRED)
target_link_libraries(myapp PRIVATE GeodesyKit::GeodesyKit)
```

### Optional: GeographicLib

For nanometre-level geodesic accuracy (Karney's algorithm), enable GeographicLib:

```bash
cmake -B build -DGEODESYKIT_USE_GEOGRAPHICLIB=ON
```

Install GeographicLib first:

```bash
# Ubuntu / Debian
sudo apt install libgeographic-dev

# macOS
brew install geographiclib

# Windows (vcpkg)
vcpkg install geographiclib
```

When `GEODESYKIT_USE_GEOGRAPHICLIB` is defined, `geodesicInverse()` automatically
dispatches to `GeographicLib::Geodesic::Inverse()`. Otherwise, it falls back to
the built-in Vincenty implementation.

---

## API Overview

### Coordinate Transforms

```cpp
#include <geodesykit/coordinate.hpp>

// Geodetic (lat deg, lon deg, h [m]) -> ECEF (x, y, z) [m]
Vec3 ecef = geodetic2ecef(lat, lon, h);

// ECEF -> Geodetic (Vermeille, machine precision)
double lat, lon, h;
ecef2geodetic(ecef.x, ecef.y, ecef.z, lat, lon, h);

// Local East-North-Up axes at a geodetic location
Vec3 E, N, U;
enuAxes(lat, lon, E, N, U);
```

### Geodesic Distance

```cpp
#include <geodesykit/distance.hpp>

// Geodesic distance along WGS84 ellipsoid
GeodesicResult g = geodesicInverse(lat1, lon1, lat2, lon2);
// g.distance  - meters
// g.azimuth1  - forward azimuth at point 1 [deg]
// g.azimuth2  - forward azimuth at point 2 [deg]
// g.converged - bool

// 3D distance and bearing between two geodetic points (ENU-based)
DistanceBearing db = geoDistanceBearing(lat1, lon1, alt1,
                                        lat2, lon2, alt2);
// db.distance  - 3D chord [m]
// db.azimuth   - [0, 360) [deg]
// db.elevation - [-90, 90] [deg]
```

> **Note**: `geoDistanceBearing` uses a flat-Earth (ENU) approximation
> valid for distances up to a few hundred kilometres. For longer
> distances, use `geodesicInverse()`.

### 3D Triangulation

```cpp
#include <geodesykit/triangulation.hpp>

// Weighted least-squares triangulation with Tikhonov regularization
TriangulationResult r = triangulate3D(
    lat1, lon1, h1, az1, el1,   // observer 1
    lat2, lon2, h2, az2, el2,   // observer 2
    weight1, weight2,           // inverse variances
    lambda_reg);                // regularization parameter

// r.lat, r.lon, r.alt   - triangulated point
// r.residual            - distance between the two rays' closest points
// r.range1, r.range2    - slant ranges from each observer
// r.condition           - condition number estimate
// r.degenerate          - true if the geometry is ill-conditioned
//                         or inputs are invalid
```

**Tuning tips**:

- `weight1` / `weight2` — set to the inverse variance of each observation.
  For example, if azimuth standard deviation is 0.1 deg, use weight ≈ 100.
- `lambda_reg` — recommended `1e-8` to `1e-6`. Increase if `u1 · u2` is
  close to 1 (nearly parallel rays).
- Invalid inputs (negative weights, negative `lambda_reg`, degenerate
  geometry) are reported via `r.degenerate = true`.

---

## Examples

A complete example is provided in `examples/basic_usage.cpp`:

```cpp
#include <geodesykit/geodesykit.hpp>
#include <cstdio>

int main()
{
    using namespace geodesykit;

    // NRW locations
    const double lat_Siegen = 50.8748,  lon_Siegen = 8.0243;
    const double lat_Dom    = 50.94131, lon_Dom    = 6.95811;

    // Geodesic distance
    auto g = geodesicInverse(lat_Siegen, lon_Siegen, lat_Dom, lon_Dom);
    std::printf("Siegen -> Koelner Dom: %.3f km\n", g.distance / 1000.0);
    std::printf("Forward azimuth: %.6f deg\n", g.azimuth1);

    // Triangulation from two NRW observers
    auto tri = triangulate3D(
        50.913172, 8.005806, 550.0,  45.0, 10.0,   // Kalteiche
        50.935689, 8.013476, 400.0, 225.0,  8.0,   // Kreuztal
        1.0, 1.0, 1e-6);

    std::printf("Triangulated: lat=%.8f lon=%.8f alt=%.3f m\n",
                tri.lat, tri.lon, tri.alt);
    return 0;
}
```

Build and run:

```bash
g++ -std=c++17 -I include examples/basic_usage.cpp -o example
./example
```

---

## Building & Testing

```bash
# Configure
cmake -B build \
    -DGEODESYKIT_BUILD_TESTS=ON \
    -DGEODESYKIT_BUILD_EXAMPLES=ON

# Build
cmake --build build -j

# Run tests
ctest --test-dir build --output-on-failure

# Run example
./build/geodesykit_example
```

To enable GeographicLib-backed geodesics:

```bash
cmake -B build -DGEODESYKIT_USE_GEOGRAPHICLIB=ON
```

The test suite covers:

- Round-trip precision for NRW locations (Siegen, Koelner Dom, Zeche Zollverein).
- Geodesic distances between NRW landmarks.
- Reference values (equator quarter-circumference, symmetry).
- Triangulation with realistic NRW observer geometry.
- Weighted vs. unweighted triangulation.
- Degenerate (near-parallel) ray detection.
- Input validation for invalid weights and regularization parameters.

---

## Project Structure

```
geodesykit/
├── include/
│   └── geodesykit/
│       ├── geodesykit.hpp       # Umbrella header
│       ├── constants.hpp        # WGS84 constants
│       ├── vec3.hpp             # 3D vector primitives
│       ├── vermeille.hpp        # Vermeille ECEF -> geodetic
│       ├── coordinate.hpp       # geodetic <-> ECEF, ENU axes
│       ├── vincenty.hpp         # Vincenty inverse (fallback)
│       ├── distance.hpp         # Geodesic & 3D distance
│       └── triangulation.hpp    # Weighted LSQ triangulation
├── tests/
│   ├── test_common.hpp          # REQUIRE / REQUIRE_NEAR macros
│   ├── test_coordinate.cpp
│   ├── test_distance.cpp
│   └── test_triangulation.cpp
├── examples/
│   └── basic_usage.cpp
├── cmake/
│   └── GeodesyKitConfig.cmake.in
├── CMakeLists.txt
├── CITATION.cff
├── CHANGELOG.md
├── LICENSE
└── README.md
```

---

## References

1. **H. Vermeille**, *Direct transformation from geocentric coordinates to geodetic coordinates*,
   Journal of Geodesy, **76**(8):451–454, 2002.
   [doi:10.1007/s00190-002-0273-6](https://doi.org/10.1007/s00190-002-0273-6)

2. **H. Vermeille**, *Computing geodetic coordinates from geocentric coordinates*,
   Journal of Geodesy, **78**(1):94–95, 2004.
   [doi:10.1007/s00190-003-0364-z](https://doi.org/10.1007/s00190-003-0364-z)

3. **C. F. F. Karney**, *Algorithms for geodesics*,
   Journal of Geodesy, **87**(1):43–55, 2013.
   [doi:10.1007/s00190-012-0578-z](https://doi.org/10.1007/s00190-012-0578-z)

4. **T. Vincenty**, *Direct and inverse solutions of geodesics on the ellipsoid
   with application of nested equations*,
   Survey Review, **23**(176):88–93, 1975.
   [doi:10.1179/sre.1975.23.176.88](https://doi.org/10.1179/sre.1975.23.176.88)

5. **C. F. F. Karney**, *GeographicLib*,
   [https://geographiclib.sourceforge.io/](https://geographiclib.sourceforge.io/)

6. **NIMA**, *Department of Defense World Geodetic System 1984*,
   Technical Report TR8350.2, 3rd ed., 2000.

---

## Citation

If you use GeodesyKit in academic work, please cite it as:

```bibtex
@software{alikhani2026geodesykit,
  author       = {Alikhani, Mohammad Hossein},
  title        = {{GeodesyKit}: A Modern {C++} Geodesy Toolkit},
  year         = {2026},
  version      = {0.1.0},
  publisher    = {Zenodo},
  doi          = {10.5281/zenodo.XXXXXXX},
  url          = {https://github.com/MHAlikhani/geodesykit}
}
```

> Replace `XXXXXXX` with the DOI assigned by Zenodo after your first tagged release.

---

## Contributing

Contributions are welcome. Please:

1. Fork the repository.
2. Create a feature branch (`git checkout -b feature/my-feature`).
3. Add tests for any new functionality.
4. Ensure `ctest` passes.
5. Submit a pull request.

For bug reports, please include:

- Compiler and version.
- Operating system.
- A minimal reproducible example.
- Expected vs. observed behaviour.

---

## Author

**Mohammad Hossein Alikhani**
Siegen, North Rhine-Westphalia, Germany

- GitHub: [@MHAlikhani](https://github.com/MHAlikhani)
- ORCID: [0009-0007-3942-2656](https://orcid.org/0009-0007-3942-2656)
- Email: mohammad.hosein.alikhani08@gmail.com

---

## License

This project is licensed under the **MIT License**.
See the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2026 Mohammad Hossein Alikhani

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```