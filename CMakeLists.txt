cmake_minimum_required(VERSION 3.10)
project(LightNBT VERSION 0.1 LANGUAGES CXX)

add_library(LightNBT INTERFACE)
add_library(LightNBT::LightNBT ALIAS LightNBT)
target_include_directories(
    LightNBT INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(LightNBT INTERFACE cxx_std_23)

include(GNUInstallDirs)
install(
    FILES lnbt.hpp lmca.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS LightNBT EXPORT LightNBTTargets)
install(
    EXPORT LightNBTTargets
    FILE LightNBTConfig.cmake
    NAMESPACE LightNBT::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LightNBT
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    LightNBTConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/LightNBTConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LightNBTConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LightNBT
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LightNBTConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/LightNBTConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LightNBT
)