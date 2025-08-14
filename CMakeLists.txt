cmake_minimum_required(VERSION 3.20)

# Project configuration
project(AutonomousPhysicalTherapyDevice
    VERSION 1.0.0
    DESCRIPTION "IEC 62304 Class C Medical Device Software"
    LANGUAGES CXX C
)

# Set C++ standard and compliance requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# IEC 62304 Class C compliance settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # For static analysis tools
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)  # Link Time Optimization

# Build types and configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type" FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

# Platform detection and cross-compilation support
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(TARGET_PLATFORM "ARM64")
    set(IS_JETSON ON)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(TARGET_PLATFORM "X86_64")
    set(IS_JETSON OFF)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "Target platform: ${TARGET_PLATFORM}")
message(STATUS "Is Jetson: ${IS_JETSON}")

# Compiler flags for IEC 62304 Class C compliance
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Warning flags for medical device compliance
    set(MEDICAL_DEVICE_WARNINGS
        -Wall -Wextra -Wpedantic
        -Werror                    # Treat warnings as errors
        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wdouble-promotion
        -Wfloat-equal
        -Wformat=2
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wshadow
        -Wsign-conversion
        -Wundef
        -Wunreachable-code
        -Wwrite-strings
    )
    
    # Security hardening flags
    set(SECURITY_FLAGS
        -fstack-protector-strong
        -fPIE
        -D_FORTIFY_SOURCE=2
        -Wformat-security
    )
    
    # Performance and optimization flags
    set(OPTIMIZATION_FLAGS
        -O2                        # Optimize for performance
        -DNDEBUG                   # Remove debug assertions in release
        -ffunction-sections        # Enable dead code elimination
        -fdata-sections
    )
    
    # Add flags based on build type
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g3 -O0 -DDEBUG)
        add_link_options(-g3)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(${OPTIMIZATION_FLAGS})
        add_link_options(-Wl,--gc-sections)  # Link-time dead code elimination
    endif()
    
    # Apply medical device compliance flags
    add_compile_options(${MEDICAL_DEVICE_WARNINGS} ${SECURITY_FLAGS})
    add_link_options(${SECURITY_FLAGS})
endif()

# CUDA support for Jetson Orin NX
if(IS_JETSON)
    enable_language(CUDA)
    find_package(CUDA REQUIRED)
    
    if(CUDA_FOUND)
        set(CMAKE_CUDA_STANDARD 17)
        set(CMAKE_CUDA_STANDARD_REQUIRED ON)
        message(STATUS "CUDA Version: ${CUDA_VERSION}")
        
        # TensorRT support for AI acceleration
        find_path(TENSORRT_INCLUDE_DIR NvInfer.h
            HINTS ${CUDA_TOOLKIT_ROOT_DIR}
            PATH_SUFFIXES include)
        
        find_library(TENSORRT_LIBRARY nvinfer
            HINTS ${CUDA_TOOLKIT_ROOT_DIR}
            PATH_SUFFIXES lib lib64 lib/x64)
        
        if(TENSORRT_INCLUDE_DIR AND TENSORRT_LIBRARY)
            set(TENSORRT_FOUND ON)
            message(STATUS "TensorRT found: ${TENSORRT_LIBRARY}")
        else()
            message(WARNING "TensorRT not found - AI acceleration will be limited")
        endif()
    endif()
endif()

# Find required packages
find_package(PkgConfig REQUIRED)

# OpenCV for computer vision
find_package(OpenCV 4.0 REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
endif()

# Intel RealSense SDK (for D435 camera)
find_package(realsense2 QUIET)
if(realsense2_FOUND)
    message(STATUS "Intel RealSense SDK found")
    set(HAS_REALSENSE ON)
else()
    message(WARNING "Intel RealSense SDK not found - D435 support disabled")
    set(HAS_REALSENSE OFF)
endif()

# ORBBEC SDK (for Femto Mega camera)
find_path(ORBBEC_INCLUDE_DIR OrbbecSDK.h
    PATHS /usr/local/include /usr/include
    PATH_SUFFIXES OrbbecSDK)

find_library(ORBBEC_LIBRARY OrbbecSDK
    PATHS /usr/local/lib /usr/lib
    PATH_SUFFIXES OrbbecSDK)

if(ORBBEC_INCLUDE_DIR AND ORBBEC_LIBRARY)
    set(HAS_ORBBEC ON)
    message(STATUS "ORBBEC SDK found: ${ORBBEC_LIBRARY}")
else()
    message(WARNING "ORBBEC SDK not found - Femto Mega support disabled")
    set(HAS_ORBBEC OFF)
endif()

# MediaPipe for AI processing
find_path(MEDIAPIPE_INCLUDE_DIR mediapipe/framework/calculator_framework.h
    PATHS /usr/local/include /usr/include)

find_library(MEDIAPIPE_LIBRARY mediapipe
    PATHS /usr/local/lib /usr/lib)

if(MEDIAPIPE_INCLUDE_DIR AND MEDIAPIPE_LIBRARY)
    set(HAS_MEDIAPIPE ON)
    message(STATUS "MediaPipe found: ${MEDIAPIPE_LIBRARY}")
else()
    message(WARNING "MediaPipe not found - building minimal AI support")
    set(HAS_MEDIAPIPE OFF)
endif()

# Threading support
find_package(Threads REQUIRED)

# Networking libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(WEBSOCKET QUIET libwebsockets)
if(WEBSOCKET_FOUND)
    message(STATUS "libwebsockets found")
    set(HAS_WEBSOCKET ON)
else()
    message(WARNING "libwebsockets not found - WebSocket support disabled")
    set(HAS_WEBSOCKET OFF)
endif()

# Testing framework
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    message(STATUS "Google Test found - testing enabled")
    set(ENABLE_TESTING ON)
else()
    message(WARNING "Google Test not found - testing disabled")
    set(ENABLE_TESTING OFF)
endif()

# Configuration header generation
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/config.h"
    @ONLY
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# Add third-party libraries
add_subdirectory(third_party)

# Add source directories
add_subdirectory(src/cpp)

# Add Python support (optional)
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Python3 found: ${Python3_VERSION}")
    add_subdirectory(src/python)
endif()

# Add testing if enabled
if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

# Documentation generation (Doxygen)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Installation rules
include(GNUInstallDirs)

# Install binaries
install(TARGETS therapy_device
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Runtime
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/therapy_device
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install configuration files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config/
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/therapy_device
    COMPONENT Runtime
    FILES_MATCHING PATTERN "*.json" PATTERN "*.yaml" PATTERN "*.xml"
)

# Create package configuration
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TherapyDeviceConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/TherapyDeviceConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TherapyDevice
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/TherapyDeviceConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/TherapyDeviceConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/TherapyDeviceConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TherapyDevice
    COMPONENT Development
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "autonomous-physical-therapy-device")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Medical Device Company")
set(CPACK_PACKAGE_CONTACT "support@company.com")

# Platform-specific packaging
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    
    # Debian package configuration
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Medical Device Team")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    
    # RPM package configuration
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Science")
    set(CPACK_RPM_PACKAGE_LICENSE "Proprietary")
endif()

include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "=================== Configuration Summary ===================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Target platform: ${TARGET_PLATFORM}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
message(STATUS "Hardware Support:")
message(STATUS "  Intel RealSense D435: ${HAS_REALSENSE}")
message(STATUS "  ORBBEC Femto Mega: ${HAS_ORBBEC}")
message(STATUS "  CUDA/TensorRT: ${TENSORRT_FOUND}")
message(STATUS "")
message(STATUS "Software Components:")
message(STATUS "  OpenCV: ${OpenCV_VERSION}")
message(STATUS "  MediaPipe: ${HAS_MEDIAPIPE}")
message(STATUS "  WebSocket: ${HAS_WEBSOCKET}")
message(STATUS "  Testing: ${ENABLE_TESTING}")
message(STATUS "")
message(STATUS "IEC 62304 Class C Compliance:")
message(STATUS "  Warnings as errors: ON")
message(STATUS "  Static analysis support: ON")
message(STATUS "  Security hardening: ON")
message(STATUS "  Compile commands export: ON")
message(STATUS "===========================================================")
message(STATUS "")
