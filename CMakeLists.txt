cmake_minimum_required(VERSION 3.16)
project(GeodesyKit VERSION 0.1.0
    DESCRIPTION "A Modern C++ Geodesy Toolkit"
    HOMEPAGE_URL "https://github.com/MHAlikhani/geodesykit"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(GEODESYKIT_BUILD_TESTS "Build unit tests" OFF)
option(GEODESYKIT_BUILD_EXAMPLES "Build examples" OFF)
option(GEODESYKIT_USE_GEOGRAPHICLIB "Use GeographicLib for geodesics" OFF)

add_library(GeodesyKit INTERFACE)
add_library(GeodesyKit::GeodesyKit ALIAS GeodesyKit)
target_include_directories(GeodesyKit INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(GeodesyKit INTERFACE cxx_std_17)

if(GEODESYKIT_USE_GEOGRAPHICLIB)
    find_package(GeographicLib REQUIRED)
    if(NOT TARGET GeographicLib::GeographicLib)
        message(FATAL_ERROR "GeographicLib was found but did not define GeographicLib::GeographicLib. Check the package's CMake target name.")
    endif()
    target_link_libraries(GeodesyKit INTERFACE GeographicLib::GeographicLib)
    target_compile_definitions(GeodesyKit INTERFACE GEODESYKIT_USE_GEOGRAPHICLIB)
endif()

install(TARGETS GeodesyKit EXPORT GeodesyKitTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT GeodesyKitTargets FILE GeodesyKitTargets.cmake NAMESPACE GeodesyKit::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeodesyKit)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeodesyKitConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeodesyKit)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/GeodesyKitConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeodesyKit)

if(GEODESYKIT_BUILD_EXAMPLES)
    add_executable(geodesykit_example examples/basic_usage.cpp)
    target_link_libraries(geodesykit_example PRIVATE GeodesyKit::GeodesyKit)
endif()

if(GEODESYKIT_BUILD_TESTS)
    enable_testing()
    foreach(test_name IN ITEMS coordinate distance triangulation)
        add_executable(geodesykit_test_${test_name} tests/test_${test_name}.cpp)
        target_link_libraries(geodesykit_test_${test_name} PRIVATE GeodesyKit::GeodesyKit)
        add_test(NAME geodesykit_${test_name} COMMAND geodesykit_test_${test_name})
    endforeach()
endif()

set(CPACK_PACKAGE_NAME "GeodesyKit")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Mohammad Hossein Alikhani")
set(CPACK_PACKAGE_CONTACT "mohammad.hosein.alikhani08@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Modern C++ Geodesy Toolkit")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
endif()
include(CPack)
