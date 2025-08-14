# Unit Tests CMakeLists.txt
# IEC 62304 Class C Medical Device Software - Unit Testing

# Unit test sources
set(UNIT_TEST_SOURCES
    test_logger.cpp
    test_config_manager.cpp
    test_error_handler.cpp
    test_application.cpp
    test_camera_interface.cpp
    test_camera_factory.cpp
    test_d435_camera.cpp
    test_femto_mega_camera.cpp
    test_utils_unit.cpp
)

# Create unit test executable
add_executable(unit_tests ${UNIT_TEST_SOURCES})

# Include directories
target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/tests/common
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/../
)

# Compiler settings
target_compile_features(unit_tests PRIVATE cxx_std_17)
target_compile_options(unit_tests PRIVATE ${TEST_COMPILE_FLAGS})
target_compile_definitions(unit_tests PRIVATE
    THERAPY_DEVICE_UNIT_TESTING=1
    THERAPY_DEVICE_TEST_DATA_DIR="${TEST_DATA_DIR}"
)

# Link libraries
target_link_libraries(unit_tests
    PRIVATE
        ${TEST_COMMON_LIBRARIES}
        TherapyDevice::TestCommon
)

# Link coverage flags if enabled
if(TEST_LINK_FLAGS)
    target_link_options(unit_tests PRIVATE ${TEST_LINK_FLAGS})
endif()

# Set executable properties
set_target_properties(unit_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Register unit tests with CTest
add_test(NAME UnitTests COMMAND unit_tests)
set_tests_properties(UnitTests PROPERTIES
    TIMEOUT ${TESTING_TIMEOUT}
    LABELS "Unit;Medical;IEC62304"
)

# Individual test registration for granular control
add_test(NAME UnitTest_Logger COMMAND unit_tests --gtest_filter="LoggerTest.*")
add_test(NAME UnitTest_ConfigManager COMMAND unit_tests --gtest_filter="ConfigManagerTest.*")
add_test(NAME UnitTest_ErrorHandler COMMAND unit_tests --gtest_filter="ErrorHandlerTest.*")
add_test(NAME UnitTest_Application COMMAND unit_tests --gtest_filter="ApplicationTest.*")
add_test(NAME UnitTest_CameraInterface COMMAND unit_tests --gtest_filter="CameraInterfaceTest.*")
add_test(NAME UnitTest_CameraFactory COMMAND unit_tests --gtest_filter="CameraFactoryTest.*")
add_test(NAME UnitTest_D435Camera COMMAND unit_tests --gtest_filter="D435CameraTest.*")
add_test(NAME UnitTest_FemtoMegaCamera COMMAND unit_tests --gtest_filter="FemtoMegaCameraTest.*")
add_test(NAME UnitTest_Utils COMMAND unit_tests --gtest_filter="UtilsTest.*")

# Set properties for individual tests
set_tests_properties(
    UnitTest_Logger
    UnitTest_ConfigManager
    UnitTest_ErrorHandler
    UnitTest_Application
    UnitTest_CameraInterface
    UnitTest_CameraFactory
    UnitTest_D435Camera
    UnitTest_FemtoMegaCamera
    UnitTest_Utils
    PROPERTIES
        TIMEOUT ${TESTING_TIMEOUT}
        LABELS "Unit;Medical;IEC62304"
)

# Medical device compliance tests
add_test(NAME UnitTest_MedicalCompliance COMMAND unit_tests --gtest_filter="*MedicalCompliance*")
set_tests_properties(UnitTest_MedicalCompliance PROPERTIES
    TIMEOUT ${TESTING_TIMEOUT}
    LABELS "Unit;Medical;Compliance;Critical"
)

# Safety-critical tests
add_test(NAME UnitTest_Safety COMMAND unit_tests --gtest_filter="*Safety*")
set_tests_properties(UnitTest_Safety PROPERTIES
    TIMEOUT ${TESTING_TIMEOUT}
    LABELS "Unit;Safety;Critical"
)

# Performance unit tests
add_test(NAME UnitTest_Performance COMMAND unit_tests --gtest_filter="*Performance*")
set_tests_properties(UnitTest_Performance PROPERTIES
    TIMEOUT ${TESTING_TIMEOUT}
    LABELS "Unit;Performance"
)

# Install unit test executable
install(TARGETS unit_tests
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests
    COMPONENT Testing
)

# Create test data for unit tests
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)

# Generate sample test data
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_data/sample_config.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_data/sample_config.json
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_data/invalid_config.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_data/invalid_config.json
    @ONLY
)

# Custom targets for specific unit test categories
add_custom_target(unit_tests_core
    COMMAND unit_tests --gtest_filter="LoggerTest.*:ConfigManagerTest.*:ErrorHandlerTest.*:ApplicationTest.*"
    DEPENDS unit_tests
    COMMENT "Running core system unit tests"
    VERBATIM
)

add_custom_target(unit_tests_hardware
    COMMAND unit_tests --gtest_filter="*Camera*"
    DEPENDS unit_tests
    COMMENT "Running hardware unit tests"
    VERBATIM
)

add_custom_target(unit_tests_medical
    COMMAND unit_tests --gtest_filter="*Medical*:*Safety*:*Compliance*"
    DEPENDS unit_tests
    COMMENT "Running medical device unit tests"
    VERBATIM
)

add_custom_target(unit_tests_performance
    COMMAND unit_tests --gtest_filter="*Performance*"
    DEPENDS unit_tests
    COMMENT "Running performance unit tests"
    VERBATIM
)

# Unit test coverage analysis
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND LCOV_PATH)
    add_custom_target(unit_test_coverage
        COMMAND ${CMAKE_COMMAND} -E echo "Running unit tests for coverage analysis..."
        
        # Initialize coverage
        COMMAND ${LCOV_PATH} --directory . --zerocounters
        
        # Run unit tests
        COMMAND unit_tests
        
        # Capture coverage data
        COMMAND ${LCOV_PATH} --directory . --capture --output-file unit_coverage.info
        
        # Filter coverage data
        COMMAND ${LCOV_PATH} --remove unit_coverage.info 
                '/usr/*' '*/third_party/*' '*/tests/*' '*/build/*'
                --output-file unit_coverage_filtered.info
        
        # Generate HTML report
        COMMAND ${GENHTML_PATH} unit_coverage_filtered.info 
                --output-directory unit_coverage_html
                --title "Unit Test Coverage Report"
                --show-details --legend
        
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS unit_tests
        COMMENT "Generating unit test coverage report"
        VERBATIM
    )
endif()

# Unit test validation
add_custom_target(validate_unit_tests
    COMMAND ${CMAKE_COMMAND} -E echo "Validating unit test compliance..."
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/scripts/validate_unit_tests.py 
            --test-executable $<TARGET_FILE:unit_tests>
            --requirements ${CMAKE_SOURCE_DIR}/docs/requirements/unit_test_requirements.txt
    DEPENDS unit_tests
    COMMENT "Validating unit test medical device compliance"
    VERBATIM
)

# Print unit test configuration
message(STATUS "Unit Tests Configuration:")
message(STATUS "  Test executable: unit_tests")
message(STATUS "  Test sources: ${UNIT_TEST_SOURCES}")
message(STATUS "  Coverage enabled: ${LCOV_PATH}")
message(STATUS "  Medical compliance: ENABLED")
message(STATUS "  Safety testing: ENABLED")
