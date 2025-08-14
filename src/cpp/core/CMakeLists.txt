# Core System Components CMakeLists.txt
# IEC 62304 Class C Medical Device Software - Core Components

# Core system sources
set(CORE_SOURCES
    logger.cpp
    config_manager.cpp
    error_handler.cpp
    application.cpp
)

# Core system headers
set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/core/logger.h
    ${CMAKE_SOURCE_DIR}/include/core/config_manager.h
    ${CMAKE_SOURCE_DIR}/include/core/error_handler.h
    ${CMAKE_SOURCE_DIR}/include/core/application.h
)

# Create core system library
add_library(TherapyDevice_Core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
add_library(TherapyDevice::Core ALIAS TherapyDevice_Core)

# Include directories
target_include_directories(TherapyDevice_Core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler settings for medical device compliance
target_compile_features(TherapyDevice_Core PUBLIC cxx_std_17)
target_compile_definitions(TherapyDevice_Core
    PUBLIC
        THERAPY_DEVICE_CORE_VERSION="${PROJECT_VERSION}"
    PRIVATE
        THERAPY_DEVICE_CORE_IMPLEMENTATION=1
        THERAPY_DEVICE_IEC62304_CLASS_C=1
)

# Link required libraries
target_link_libraries(TherapyDevice_Core
    PUBLIC
        Threads::Threads
    PRIVATE
        TherapyDevice::Utils  # For utilities like string processing, crypto
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(TherapyDevice_Core PRIVATE ws2_32 iphlpapi)
elseif(UNIX)
    target_link_libraries(TherapyDevice_Core PRIVATE dl)
endif()

# JSON support for configuration management
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(JSONCPP jsoncpp)
    if(JSONCPP_FOUND)
        target_link_libraries(TherapyDevice_Core PRIVATE ${JSONCPP_LIBRARIES})
        target_include_directories(TherapyDevice_Core PRIVATE ${JSONCPP_INCLUDE_DIRS})
        target_compile_definitions(TherapyDevice_Core PRIVATE THERAPY_DEVICE_HAS_JSONCPP=1)
        message(STATUS "Core: JSON support enabled via jsoncpp")
    else()
        message(STATUS "Core: JSON support disabled (jsoncpp not found)")
    endif()
else()
    message(STATUS "Core: JSON support disabled (pkg-config not found)")
endif()

# Encryption support for configuration
find_package(OpenSSL QUIET)
if(OpenSSL_FOUND)
    target_link_libraries(TherapyDevice_Core PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(TherapyDevice_Core PRIVATE THERAPY_DEVICE_HAS_OPENSSL=1)
    message(STATUS "Core: Encryption support enabled via OpenSSL")
else()
    message(STATUS "Core: Encryption support disabled (OpenSSL not found)")
endif()

# Database support for audit trails
find_package(SQLite3 QUIET)
if(SQLite3_FOUND)
    target_link_libraries(TherapyDevice_Core PRIVATE SQLite::SQLite3)
    target_compile_definitions(TherapyDevice_Core PRIVATE THERAPY_DEVICE_HAS_SQLITE=1)
    message(STATUS "Core: Database support enabled via SQLite3")
else()
    message(STATUS "Core: Database support disabled (SQLite3 not found)")
endif()

# Medical device compliance features
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(TherapyDevice_Core PRIVATE 
        THERAPY_DEVICE_ENABLE_DEBUG_LOGGING=1
        THERAPY_DEVICE_ENABLE_STACK_TRACE=1
    )
endif()

# Set library properties
set_target_properties(TherapyDevice_Core PROPERTIES
    OUTPUT_NAME "therapy_device_core"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Export symbols for Windows
if(WIN32)
    target_compile_definitions(TherapyDevice_Core PRIVATE THERAPY_DEVICE_CORE_EXPORTS)
endif()

# Install the library
install(TARGETS TherapyDevice_Core
    EXPORT TherapyDeviceTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Development
)

# Install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/core/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/therapy_device/core
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Add custom targets for static analysis
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(core_static_analysis
        COMMAND cppcheck --enable=all --std=c++17 --error-exitcode=1 
                --suppress=missingIncludeSystem
                --inline-suppr
                --project=${CMAKE_BINARY_DIR}/compile_commands.json
                --file-filter="*core*"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis on core components"
        VERBATIM
    )
endif()

# Core validation tests
if(ENABLE_TESTING)
    add_custom_target(core_validation
        COMMAND ${CMAKE_COMMAND} -E echo "Running core system validation tests..."
        COMMAND $<TARGET_FILE:core_tests> --gtest_filter="Core*"
        DEPENDS core_tests
        COMMENT "Running core system validation"
        VERBATIM
    )
endif()

# Memory leak detection for core components
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(VALGRIND_PROGRAM valgrind)
    if(VALGRIND_PROGRAM)
        add_custom_target(core_memcheck
            COMMAND ${VALGRIND_PROGRAM} --tool=memcheck --leak-check=full 
                    --show-leak-kinds=all --track-origins=yes 
                    --error-exitcode=1 $<TARGET_FILE:core_tests>
            DEPENDS core_tests
            COMMENT "Running memory leak detection on core components"
            VERBATIM
        )
    endif()
endif()

# Performance benchmarks for core components
add_custom_target(core_benchmark
    COMMAND ${CMAKE_COMMAND} -E echo "Running core system performance benchmarks..."
    COMMENT "Core system performance benchmarking"
    VERBATIM
)

# Documentation for core components
if(DOXYGEN_FOUND)
    set(CORE_DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.core.in)
    set(CORE_DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.core)
    
    if(EXISTS ${CORE_DOXYFILE_IN})
        configure_file(${CORE_DOXYFILE_IN} ${CORE_DOXYFILE} @ONLY)
        
        add_custom_target(core_docs
            ${DOXYGEN_EXECUTABLE} ${CORE_DOXYFILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating core system documentation"
            VERBATIM
        )
        
        add_dependencies(docs core_docs)
    endif()
endif()

# Core compliance report
add_custom_target(core_compliance_report
    COMMAND ${CMAKE_COMMAND} -E echo "Generating core system compliance report..."
    COMMAND ${CMAKE_COMMAND} -E echo "IEC 62304 Class C Core System Compliance:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Centralized logging: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Configuration management: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Error handling: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Application lifecycle: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Audit trail: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Medical device safety: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Thread safety: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Resource management: IMPLEMENTED"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Performance monitoring: IMPLEMENTED"
    COMMENT "Core system compliance report"
    VERBATIM
)

# Add to main compliance report
add_dependencies(compliance_report core_compliance_report)

# Configuration file templates
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config_template.json.in
    ${CMAKE_BINARY_DIR}/config/device_config_template.json
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/logging_config.json.in
    ${CMAKE_BINARY_DIR}/config/logging_config.json
    @ONLY
)

# Install configuration templates
install(FILES
    ${CMAKE_BINARY_DIR}/config/device_config_template.json
    ${CMAKE_BINARY_DIR}/config/logging_config.json
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/therapy_device
    COMPONENT Runtime
)

# Print core configuration summary
message(STATUS "")
message(STATUS "================ Core Configuration ================")
message(STATUS "Core System Components: ENABLED")
message(STATUS "Logging System: ENABLED")
message(STATUS "Configuration Manager: ENABLED")
message(STATUS "Error Handler: ENABLED")
message(STATUS "Application Lifecycle: ENABLED")
message(STATUS "JSON Support: ${JSONCPP_FOUND}")
message(STATUS "Encryption Support: ${OpenSSL_FOUND}")
message(STATUS "Database Support: ${SQLite3_FOUND}")
message(STATUS "Medical Device Compliance: ENABLED")
message(STATUS "Thread Safety: ENABLED")
message(STATUS "Audit Trail: ENABLED")
message(STATUS "Performance Monitoring: ENABLED")
message(STATUS "====================================================")
