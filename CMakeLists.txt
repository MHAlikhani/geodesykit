cmake_minimum_required(VERSION 3.16)

project(GeodesyKit
    VERSION 0.1.0
    DESCRIPTION "A Modern C++ Geodesy Toolkit"
    HOMEPAGE_URL "https://github.com/MHAlikhani/geodesykit"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Options ----
option(GEODESYKIT_BUILD_TESTS       "Build unit tests"        OFF)
option(GEODESYKIT_BUILD_EXAMPLES    "Build examples"          OFF)
option(GEODESYKIT_USE_GEOGRAPHICLIB "Use GeographicLib for geodesics" OFF)

# ---- Library target (header-only) ----
add_library(GeodesyKit INTERFACE)
add_library(GeodesyKit::GeodesyKit ALIAS GeodesyKit)

target_include_directories(GeodesyKit INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(GeodesyKit INTERFACE cxx_std_17)

if(GEODESYKIT_USE_GEOGRAPHICLIB)
    find_package(GeographicLib REQUIRED)
    target_link_libraries(GeodesyKit INTERFACE GeographicLib::GeographicLib)
    target_compile_definitions(GeodesyKit INTERFACE GEODESYKIT_USE_GEOGRAPHICLIB)
endif()

# ---- Install ----
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS GeodesyKit EXPORT GeodesyKitTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT GeodesyKitTargets
    FILE GeodesyKitTargets.cmake
    NAMESPACE GeodesyKit::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeodesyKit)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeodesyKitConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeodesyKit)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeodesyKit)

# ---- Examples ----
if(GEODESYKIT_BUILD_EXAMPLES)
    add_executable(geodesykit_example examples/basic_usage.cpp)
    target_link_libraries(geodesykit_example PRIVATE GeodesyKit::GeodesyKit)
endif()

# ---- Tests ----
if(GEODESYKIT_BUILD_TESTS)
    enable_testing()
    add_executable(geodesykit_tests
        tests/test_coordinate.cpp
        tests/test_distance.cpp
        tests/test_triangulation.cpp)
    target_link_libraries(geodesykit_tests PRIVATE GeodesyKit::GeodesyKit)
    add_test(NAME geodesykit_tests COMMAND geodesykit_tests)
endif()

# ---- CPack ----
set(CPACK_PACKAGE_NAME "GeodesyKit")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Mohammad Hossein Alikhani")
set(CPACK_PACKAGE_CONTACT "mohammad.hosein.alikhani08@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Modern C++ Geodesy Toolkit")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)