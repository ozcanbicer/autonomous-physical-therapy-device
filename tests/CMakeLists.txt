# Testing Framework CMakeLists.txt
# IEC 62304 Class C Medical Device Software - Comprehensive Testing

# Enable testing
enable_testing()

# Find GoogleTest
find_package(GTest REQUIRED)
if(NOT GTest_FOUND)
    message(FATAL_ERROR "GoogleTest is required for IEC 62304 Class C compliance testing")
endif()

# Find GoogleMock (usually comes with GoogleTest)
find_package(GMock QUIET)

# Testing configuration
set(TESTING_TIMEOUT 300)  # 5 minutes timeout for tests
set(TESTING_COVERAGE_THRESHOLD 95)  # 95% coverage requirement for medical device

# Common test libraries and settings
set(TEST_COMMON_LIBRARIES
    GTest::gtest
    GTest::gtest_main
    TherapyDevice::Core
    TherapyDevice::Hardware
    TherapyDevice::Utils
    Threads::Threads
)

if(GMock_FOUND)
    list(APPEND TEST_COMMON_LIBRARIES GMock::gmock GMock::gmock_main)
    add_compile_definitions(THERAPY_DEVICE_HAS_GMOCK=1)
endif()

# Test include directories
set(TEST_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests/common
    ${CMAKE_BINARY_DIR}/include
)

# Compiler flags for testing
set(TEST_COMPILE_FLAGS
    -DTHERAPY_DEVICE_TESTING=1
    -DTHERAPY_DEVICE_IEC62304_TESTING=1
    -g  # Debug symbols for coverage
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    list(APPEND TEST_COMPILE_FLAGS
        --coverage          # Enable coverage
        -fprofile-arcs      # Generate coverage data
        -ftest-coverage     # Enable test coverage
    )
    set(TEST_LINK_FLAGS --coverage)
endif()

# Test data and fixtures directory
set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(TEST_FIXTURES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fixtures)

# Create test data directory if it doesn't exist
file(MAKE_DIRECTORY ${TEST_DATA_DIR})
file(MAKE_DIRECTORY ${TEST_FIXTURES_DIR})

# Configure test data paths
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_config.h
    @ONLY
)

# Add test subdirectories
add_subdirectory(common)     # Common test utilities and mocks
add_subdirectory(unit)       # Unit tests
add_subdirectory(integration) # Integration tests  
add_subdirectory(performance) # Performance tests

# Create main test executable that runs all tests
set(ALL_TESTS_SOURCES
    main_test.cpp
)

add_executable(all_tests ${ALL_TESTS_SOURCES})

target_include_directories(all_tests PRIVATE ${TEST_INCLUDE_DIRS})
target_compile_options(all_tests PRIVATE ${TEST_COMPILE_FLAGS})
target_link_libraries(all_tests PRIVATE ${TEST_COMMON_LIBRARIES})

if(TEST_LINK_FLAGS)
    target_link_options(all_tests PRIVATE ${TEST_LINK_FLAGS})
endif()

# Set test properties
set_target_properties(all_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Register the main test
add_test(NAME AllTests COMMAND all_tests)
set_tests_properties(AllTests PROPERTIES TIMEOUT ${TESTING_TIMEOUT})

# Coverage analysis
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    
    # Find coverage tools
    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
        
        # Create coverage directory
        set(COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
        file(MAKE_DIRECTORY ${COVERAGE_DIR})
        
        # Coverage target
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E echo "Running tests for coverage..."
            
            # Initialize coverage
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            
            # Run tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            
            # Capture coverage data
            COMMAND ${LCOV_PATH} --directory . --capture --output-file ${COVERAGE_DIR}/coverage.info
            
            # Remove external libraries from coverage
            COMMAND ${LCOV_PATH} --remove ${COVERAGE_DIR}/coverage.info 
                    '/usr/*' '*/third_party/*' '*/tests/*' '*/build/*'
                    --output-file ${COVERAGE_DIR}/coverage_filtered.info
            
            # Generate HTML report
            COMMAND ${GENHTML_PATH} ${COVERAGE_DIR}/coverage_filtered.info 
                    --output-directory ${COVERAGE_DIR}/html
                    --title "Therapy Device Coverage Report"
                    --show-details --legend
            
            # Check coverage threshold
            COMMAND ${CMAKE_COMMAND} -E echo "Checking coverage threshold..."
            COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_coverage.py 
                    ${COVERAGE_DIR}/coverage_filtered.info ${TESTING_COVERAGE_THRESHOLD}
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS all_tests unit_tests integration_tests
            COMMENT "Generating coverage report"
            VERBATIM
        )
        
        # Coverage summary target
        add_custom_target(coverage_summary
            COMMAND ${LCOV_PATH} --summary ${COVERAGE_DIR}/coverage_filtered.info
            DEPENDS coverage
            COMMENT "Displaying coverage summary"
            VERBATIM
        )
        
        message(STATUS "Coverage analysis enabled - target: coverage")
        
    else()
        message(WARNING "Coverage tools not found - coverage analysis disabled")
    endif()
    
endif()

# Memory leak detection
find_program(VALGRIND_PATH valgrind)
if(VALGRIND_PATH)
    
    add_custom_target(memcheck
        COMMAND ${VALGRIND_PATH} 
                --tool=memcheck 
                --leak-check=full 
                --show-leak-kinds=all 
                --track-origins=yes 
                --verbose 
                --error-exitcode=1
                --suppressions=${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp
                $<TARGET_FILE:all_tests>
        DEPENDS all_tests
        COMMENT "Running memory leak detection"
        VERBATIM
    )
    
    message(STATUS "Memory leak detection enabled - target: memcheck")
    
endif()

# Thread sanitizer (for detecting race conditions)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    
    add_executable(all_tests_tsan ${ALL_TESTS_SOURCES})
    target_include_directories(all_tests_tsan PRIVATE ${TEST_INCLUDE_DIRS})
    target_compile_options(all_tests_tsan PRIVATE ${TEST_COMPILE_FLAGS} -fsanitize=thread -g)
    target_link_libraries(all_tests_tsan PRIVATE ${TEST_COMMON_LIBRARIES})
    target_link_options(all_tests_tsan PRIVATE -fsanitize=thread)
    
    set_target_properties(all_tests_tsan PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
    
    add_custom_target(thread_sanitizer
        COMMAND $<TARGET_FILE:all_tests_tsan>
        DEPENDS all_tests_tsan
        COMMENT "Running thread sanitizer tests"
        VERBATIM
    )
    
    message(STATUS "Thread sanitizer enabled - target: thread_sanitizer")
    
endif()

# Address sanitizer (for detecting memory errors)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    
    add_executable(all_tests_asan ${ALL_TESTS_SOURCES})
    target_include_directories(all_tests_asan PRIVATE ${TEST_INCLUDE_DIRS})
    target_compile_options(all_tests_asan PRIVATE ${TEST_COMPILE_FLAGS} -fsanitize=address -g)
    target_link_libraries(all_tests_asan PRIVATE ${TEST_COMMON_LIBRARIES})
    target_link_options(all_tests_asan PRIVATE -fsanitize=address)
    
    set_target_properties(all_tests_asan PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
    
    add_custom_target(address_sanitizer
        COMMAND $<TARGET_FILE:all_tests_asan>
        DEPENDS all_tests_asan
        COMMENT "Running address sanitizer tests"
        VERBATIM
    )
    
    message(STATUS "Address sanitizer enabled - target: address_sanitizer")
    
endif()

# Continuous testing target
add_custom_target(continuous_testing
    COMMAND ${CMAKE_COMMAND} -E echo "Running continuous testing suite..."
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --repeat until-fail:3
    DEPENDS all_tests
    COMMENT "Continuous testing for stability validation"
    VERBATIM
)

# Medical device compliance testing
add_custom_target(compliance_testing
    COMMAND ${CMAKE_COMMAND} -E echo "Running IEC 62304 Class C compliance tests..."
    
    # Run all tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    
    # Generate coverage report
    COMMAND ${CMAKE_COMMAND} --build . --target coverage
    
    # Run static analysis
    COMMAND ${CMAKE_COMMAND} --build . --target static_analysis
    
    # Run memory checks
    COMMAND ${CMAKE_COMMAND} --build . --target memcheck
    
    # Generate compliance report
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_compliance_report.py
            --coverage ${CMAKE_BINARY_DIR}/coverage/coverage_filtered.info
            --tests ${CMAKE_BINARY_DIR}/Testing/Temporary/LastTest.log
            --output ${CMAKE_BINARY_DIR}/compliance_report.html
    
    DEPENDS all_tests unit_tests integration_tests performance_tests
    COMMENT "IEC 62304 Class C compliance testing"
    VERBATIM
)

# Benchmark and performance validation
add_custom_target(performance_validation
    COMMAND ${CMAKE_COMMAND} -E echo "Running performance validation..."
    COMMAND $<TARGET_FILE:performance_tests> --benchmark_format=json 
            --benchmark_out=${CMAKE_BINARY_DIR}/performance_results.json
    DEPENDS performance_tests
    COMMENT "Performance validation for medical device requirements"
    VERBATIM
)

# Test report generation
add_custom_target(test_reports
    COMMAND ${CMAKE_COMMAND} -E echo "Generating comprehensive test reports..."
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_test_report.py
            --junit ${CMAKE_BINARY_DIR}/Testing/Temporary/LastTest.log
            --coverage ${CMAKE_BINARY_DIR}/coverage/coverage_filtered.info
            --performance ${CMAKE_BINARY_DIR}/performance_results.json
            --output ${CMAKE_BINARY_DIR}/test_report.html
    DEPENDS compliance_testing performance_validation
    COMMENT "Generating comprehensive test reports"
    VERBATIM
)

# Install test executables for deployment testing
install(TARGETS all_tests
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests
    COMPONENT Testing
)

# Install test data and configuration
install(DIRECTORY ${TEST_DATA_DIR}/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/therapy_device/test_data
    COMPONENT Testing
)

# Print testing configuration summary
message(STATUS "")
message(STATUS "================ Testing Configuration ================")
message(STATUS "GoogleTest: ${GTest_FOUND}")
message(STATUS "GoogleMock: ${GMock_FOUND}")
message(STATUS "Coverage Analysis: ${GCOV_PATH}")
message(STATUS "Memory Leak Detection: ${VALGRIND_PATH}")
message(STATUS "Thread Sanitizer: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Address Sanitizer: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Coverage Threshold: ${TESTING_COVERAGE_THRESHOLD}%")
message(STATUS "Test Timeout: ${TESTING_TIMEOUT} seconds")
message(STATUS "IEC 62304 Class C Compliance: ENABLED")
message(STATUS "Medical Device Testing: ENABLED")
message(STATUS "Performance Validation: ENABLED")
message(STATUS "=======================================================")
