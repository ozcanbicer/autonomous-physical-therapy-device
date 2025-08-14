# Common Test Utilities CMakeLists.txt
# IEC 62304 Class C Medical Device Software - Test Utilities

# Common test utility sources
set(TEST_COMMON_SOURCES
    test_utils.cpp
    mock_camera.cpp
    mock_logger.cpp
    mock_config_manager.cpp
    test_fixtures.cpp
    medical_test_base.cpp
    performance_test_base.cpp
)

# Common test utility headers
set(TEST_COMMON_HEADERS
    test_utils.h
    mock_camera.h
    mock_logger.h
    mock_config_manager.h
    test_fixtures.h
    medical_test_base.h
    performance_test_base.h
    test_macros.h
)

# Create common test utilities library
add_library(TherapyDevice_TestCommon STATIC ${TEST_COMMON_SOURCES} ${TEST_COMMON_HEADERS})
add_library(TherapyDevice::TestCommon ALIAS TherapyDevice_TestCommon)

# Include directories
target_include_directories(TherapyDevice_TestCommon
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/../
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
)

# Compiler settings
target_compile_features(TherapyDevice_TestCommon PUBLIC cxx_std_17)
target_compile_definitions(TherapyDevice_TestCommon
    PUBLIC
        THERAPY_DEVICE_TESTING=1
        THERAPY_DEVICE_TEST_COMMON=1
    PRIVATE
        THERAPY_DEVICE_TEST_COMMON_IMPLEMENTATION=1
)

# Link required libraries
target_link_libraries(TherapyDevice_TestCommon
    PUBLIC
        GTest::gtest
        TherapyDevice::Core
        TherapyDevice::Hardware
        TherapyDevice::Utils
        Threads::Threads
)

# Add GoogleMock if available
if(GMock_FOUND)
    target_link_libraries(TherapyDevice_TestCommon PUBLIC GMock::gmock)
endif()

# Set library properties
set_target_properties(TherapyDevice_TestCommon PROPERTIES
    OUTPUT_NAME "therapy_device_test_common"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/tests
    POSITION_INDEPENDENT_CODE ON
)

# Install test common library
install(TARGETS TherapyDevice_TestCommon
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/tests
    COMPONENT Testing
)

# Install test common headers
install(FILES ${TEST_COMMON_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/therapy_device/tests/common
    COMPONENT Testing
)
