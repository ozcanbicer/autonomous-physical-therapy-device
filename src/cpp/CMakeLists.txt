# C++ source code CMakeLists.txt
# IEC 62304 Class C Medical Device Software

# Core library - fundamental components
add_subdirectory(core)

# Hardware abstraction layer
add_subdirectory(hardware)

# AI processing engine
add_subdirectory(ai)

# Communication layer
add_subdirectory(communication)

# Utility functions
add_subdirectory(utils)

# Main executable
set(THERAPY_DEVICE_SOURCES
    main.cpp
)

add_executable(therapy_device ${THERAPY_DEVICE_SOURCES})

# Link all component libraries
target_link_libraries(therapy_device
    PRIVATE
        TherapyDevice::Core
        TherapyDevice::Hardware
        TherapyDevice::AI
        TherapyDevice::Communication
        TherapyDevice::Utils
        ${OpenCV_LIBS}
        Threads::Threads
)

# Include directories
target_include_directories(therapy_device
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/include
)

# Compiler settings for medical device compliance
target_compile_features(therapy_device PRIVATE cxx_std_17)
target_compile_definitions(therapy_device PRIVATE
    THERAPY_DEVICE_VERSION="${PROJECT_VERSION}"
    THERAPY_DEVICE_IEC62304_CLASS_C=1
)

# Platform-specific settings
if(IS_JETSON)
    target_compile_definitions(therapy_device PRIVATE THERAPY_DEVICE_JETSON=1)
    
    if(TENSORRT_FOUND)
        target_link_libraries(therapy_device PRIVATE ${TENSORRT_LIBRARY})
        target_include_directories(therapy_device PRIVATE ${TENSORRT_INCLUDE_DIR})
    endif()
endif()

# Install the executable
install(TARGETS therapy_device
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Runtime
)

# Set executable properties for medical device compliance
set_target_properties(therapy_device PROPERTIES
    OUTPUT_NAME "therapy_device"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add custom target for static analysis
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(static_analysis
        COMMAND cppcheck --enable=all --std=c++17 --error-exitcode=1 
                --suppress=missingIncludeSystem
                --project=${CMAKE_BINARY_DIR}/compile_commands.json
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis with cppcheck"
        VERBATIM
    )
    
    add_dependencies(therapy_device static_analysis)
endif()

# Memory leak detection (Debug builds only)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(VALGRIND_PROGRAM valgrind)
    if(VALGRIND_PROGRAM)
        add_custom_target(memcheck
            COMMAND ${VALGRIND_PROGRAM} --tool=memcheck --leak-check=full 
                    --show-leak-kinds=all --track-origins=yes 
                    --error-exitcode=1 $<TARGET_FILE:therapy_device>
            DEPENDS therapy_device
            COMMENT "Running memory leak detection with Valgrind"
            VERBATIM
        )
    endif()
endif()
