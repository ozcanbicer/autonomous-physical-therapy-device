/**
 * @file test_utils.h
 * @brief Common test utilities for medical device testing
 * 
 * Comprehensive test utilities for IEC 62304 Class C medical device
 * software testing including data generation, validation, and helpers.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-TEST-001, REQ-TEST-002, REQ-TEST-003
 */

#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <functional>
#include <map>
#include <random>

#include "hardware/icamera.h"
#include "core/logger.h"
#include "core/config_manager.h"
#include "test_config.h"

namespace therapy_device {
namespace testing {

/**
 * @brief Test data generator utilities
 * 
 * Generates realistic test data for medical device testing.
 */
class TestDataGenerator {
public:
    /**
     * @brief Generate sample RGB frame
     * @param width Frame width
     * @param height Frame height
     * @param pattern Pattern type ("solid", "gradient", "checkerboard", "noise")
     * @return Generated RGB frame
     */
    static hardware::RGBFrame generateRGBFrame(int width = 640, int height = 480, 
                                              const std::string& pattern = "gradient");
    
    /**
     * @brief Generate sample depth frame
     * @param width Frame width
     * @param height Frame height
     * @param min_depth Minimum depth value in mm
     * @param max_depth Maximum depth value in mm
     * @return Generated depth frame
     */
    static hardware::DepthFrame generateDepthFrame(int width = 640, int height = 480,
                                                  float min_depth = 500.0f, float max_depth = 5000.0f);
    
    /**
     * @brief Generate synchronized frame pair
     * @param width Frame width
     * @param height Frame height
     * @return Pair of synchronized RGB and depth frames
     */
    static std::pair<hardware::RGBFrame, hardware::DepthFrame> 
    generateSynchronizedFrames(int width = 640, int height = 480);
    
    /**
     * @brief Generate camera capabilities
     * @param camera_type Camera type ("d435", "femto_mega", "simulation")
     * @return Camera capabilities structure
     */
    static hardware::CameraCapabilities generateCameraCapabilities(const std::string& camera_type);
    
    /**
     * @brief Generate test configuration
     * @param config_type Configuration type ("minimal", "full", "medical", "performance")
     * @return Configuration map
     */
    static std::map<std::string, core::ConfigValue> generateTestConfig(const std::string& config_type);
    
    /**
     * @brief Generate performance metrics
     * @param realistic If true, generates realistic values within medical device limits
     * @return Performance metrics map
     */
    static std::map<std::string, double> generatePerformanceMetrics(bool realistic = true);

private:
    static std::mt19937 random_generator_;
    static void initializeRandomGenerator();
};

/**
 * @brief Test validation utilities
 * 
 * Validates test data and results against medical device requirements.
 */
class TestValidator {
public:
    /**
     * @brief Validate RGB frame data
     * @param frame RGB frame to validate
     * @return True if frame is valid
     */
    static bool validateRGBFrame(const hardware::RGBFrame& frame);
    
    /**
     * @brief Validate depth frame data
     * @param frame Depth frame to validate
     * @return True if frame is valid
     */
    static bool validateDepthFrame(const hardware::DepthFrame& frame);
    
    /**
     * @brief Validate frame synchronization
     * @param rgb_frame RGB frame
     * @param depth_frame Depth frame
     * @param max_time_diff_ms Maximum allowed time difference in milliseconds
     * @return True if frames are properly synchronized
     */
    static bool validateFrameSynchronization(const hardware::RGBFrame& rgb_frame,
                                           const hardware::DepthFrame& depth_frame,
                                           int max_time_diff_ms = 10);
    
    /**
     * @brief Validate performance metrics
     * @param metrics Performance metrics
     * @return True if metrics meet medical device requirements
     */
    static bool validatePerformanceMetrics(const std::map<std::string, double>& metrics);
    
    /**
     * @brief Validate medical device compliance
     * @param component_name Component being tested
     * @param test_results Test results
     * @return True if component meets medical device compliance
     */
    static bool validateMedicalCompliance(const std::string& component_name,
                                        const std::map<std::string, bool>& test_results);
    
    /**
     * @brief Validate safety requirements
     * @param safety_metrics Safety metrics to validate
     * @return True if safety requirements are met
     */
    static bool validateSafetyRequirements(const std::map<std::string, double>& safety_metrics);
};

/**
 * @brief Test timing utilities
 * 
 * Utilities for timing and performance measurement in tests.
 */
class TestTimer {
public:
    /**
     * @brief Constructor - starts timing
     */
    TestTimer();
    
    /**
     * @brief Get elapsed time in milliseconds
     * @return Elapsed time in milliseconds
     */
    double getElapsedMs() const;
    
    /**
     * @brief Get elapsed time in microseconds
     * @return Elapsed time in microseconds
     */
    double getElapsedUs() const;
    
    /**
     * @brief Reset timer
     */
    void reset();
    
    /**
     * @brief Check if elapsed time is within expected range
     * @param expected_ms Expected time in milliseconds
     * @param tolerance_percent Tolerance percentage (default 10%)
     * @return True if within expected range
     */
    bool isWithinExpected(double expected_ms, double tolerance_percent = 10.0) const;
    
    /**
     * @brief Static method to measure execution time
     * @param func Function to measure
     * @return Execution time in milliseconds
     */
    template<typename Func>
    static double measureExecutionTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0;  // Convert to milliseconds
    }

private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

/**
 * @brief Test file utilities
 * 
 * Utilities for file operations in tests.
 */
class TestFileUtils {
public:
    /**
     * @brief Create temporary test directory
     * @param prefix Directory prefix
     * @return Path to created directory
     */
    static std::string createTempDirectory(const std::string& prefix = "therapy_test");
    
    /**
     * @brief Remove temporary directory
     * @param path Directory path to remove
     * @return True if removal successful
     */
    static bool removeTempDirectory(const std::string& path);
    
    /**
     * @brief Create test file with content
     * @param filename File name
     * @param content File content
     * @return True if file created successfully
     */
    static bool createTestFile(const std::string& filename, const std::string& content);
    
    /**
     * @brief Read test file content
     * @param filename File name
     * @return File content, empty if error
     */
    static std::string readTestFile(const std::string& filename);
    
    /**
     * @brief Check if test file exists
     * @param filename File name
     * @return True if file exists
     */
    static bool testFileExists(const std::string& filename);
    
    /**
     * @brief Get test data file path
     * @param filename File name relative to test data directory
     * @return Full path to test data file
     */
    static std::string getTestDataPath(const std::string& filename);
    
    /**
     * @brief Copy test file
     * @param source Source file path
     * @param destination Destination file path
     * @return True if copy successful
     */
    static bool copyTestFile(const std::string& source, const std::string& destination);
};

/**
 * @brief Test assertion macros for medical device testing
 */

// Enhanced assertions for medical device testing
#define ASSERT_MEDICAL_COMPLIANCE(condition, component) \
    ASSERT_TRUE(condition) << "Medical compliance failure in " << component

#define EXPECT_MEDICAL_COMPLIANCE(condition, component) \
    EXPECT_TRUE(condition) << "Medical compliance failure in " << component

#define ASSERT_PERFORMANCE_WITHIN_LIMITS(value, min_val, max_val, metric) \
    ASSERT_GE(value, min_val) << metric << " below minimum limit"; \
    ASSERT_LE(value, max_val) << metric << " above maximum limit"

#define EXPECT_PERFORMANCE_WITHIN_LIMITS(value, min_val, max_val, metric) \
    EXPECT_GE(value, min_val) << metric << " below minimum limit"; \
    EXPECT_LE(value, max_val) << metric << " above maximum limit"

#define ASSERT_LATENCY_COMPLIANT(latency_ms) \
    ASSERT_LE(latency_ms, medical_device::MAX_LATENCY_MS) \
    << "Latency " << latency_ms << "ms exceeds medical device limit of " \
    << medical_device::MAX_LATENCY_MS << "ms"

#define EXPECT_LATENCY_COMPLIANT(latency_ms) \
    EXPECT_LE(latency_ms, medical_device::MAX_LATENCY_MS) \
    << "Latency " << latency_ms << "ms exceeds medical device limit of " \
    << medical_device::MAX_LATENCY_MS << "ms"

#define ASSERT_ACCURACY_COMPLIANT(accuracy) \
    ASSERT_GE(accuracy, medical_device::MIN_ACCURACY) \
    << "Accuracy " << accuracy << " below medical device requirement of " \
    << medical_device::MIN_ACCURACY

#define EXPECT_ACCURACY_COMPLIANT(accuracy) \
    EXPECT_GE(accuracy, medical_device::MIN_ACCURACY) \
    << "Accuracy " << accuracy << " below medical device requirement of " \
    << medical_device::MIN_ACCURACY

#define ASSERT_FRAME_VALID(frame) \
    ASSERT_TRUE(TestValidator::validateRGBFrame(frame) || TestValidator::validateDepthFrame(frame)) \
    << "Frame validation failed"

#define EXPECT_FRAME_VALID(frame) \
    EXPECT_TRUE(TestValidator::validateRGBFrame(frame) || TestValidator::validateDepthFrame(frame)) \
    << "Frame validation failed"

#define ASSERT_FRAMES_SYNCHRONIZED(rgb_frame, depth_frame) \
    ASSERT_TRUE(TestValidator::validateFrameSynchronization(rgb_frame, depth_frame)) \
    << "Frames are not properly synchronized"

#define EXPECT_FRAMES_SYNCHRONIZED(rgb_frame, depth_frame) \
    EXPECT_TRUE(TestValidator::validateFrameSynchronization(rgb_frame, depth_frame)) \
    << "Frames are not properly synchronized"

/**
 * @brief Test environment setup and teardown
 */
class TestEnvironment : public ::testing::Environment {
public:
    void SetUp() override;
    void TearDown() override;
    
    static TestEnvironment* getInstance();

private:
    TestEnvironment() = default;
    static TestEnvironment* instance_;
    
    std::string temp_dir_;
    bool initialized_;
};

/**
 * @brief Medical device test configuration
 */
class MedicalTestConfig {
public:
    /**
     * @brief Initialize medical test configuration
     * @return True if initialization successful
     */
    static bool initialize();
    
    /**
     * @brief Get medical test configuration
     * @return Configuration map
     */
    static const std::map<std::string, core::ConfigValue>& getConfig();
    
    /**
     * @brief Check if medical mode testing is enabled
     * @return True if medical mode testing is enabled
     */
    static bool isMedicalModeEnabled();
    
    /**
     * @brief Check if hardware testing is available
     * @return True if hardware testing is available
     */
    static bool isHardwareTestingAvailable();
    
    /**
     * @brief Check if performance testing should run
     * @return True if performance testing should run
     */
    static bool shouldRunPerformanceTests();

private:
    static std::map<std::string, core::ConfigValue> config_;
    static bool initialized_;
};

/**
 * @brief Test statistics and reporting
 */
class TestStatistics {
public:
    /**
     * @brief Record test execution time
     * @param test_name Test name
     * @param execution_time_ms Execution time in milliseconds
     */
    static void recordExecutionTime(const std::string& test_name, double execution_time_ms);
    
    /**
     * @brief Record test memory usage
     * @param test_name Test name
     * @param memory_usage_mb Memory usage in MB
     */
    static void recordMemoryUsage(const std::string& test_name, double memory_usage_mb);
    
    /**
     * @brief Record test coverage
     * @param component Component name
     * @param coverage_percent Coverage percentage
     */
    static void recordCoverage(const std::string& component, double coverage_percent);
    
    /**
     * @brief Get test statistics summary
     * @return Statistics summary string
     */
    static std::string getStatisticsSummary();
    
    /**
     * @brief Generate test report
     * @param output_file Output file path
     * @return True if report generated successfully
     */
    static bool generateReport(const std::string& output_file);

private:
    static std::map<std::string, double> execution_times_;
    static std::map<std::string, double> memory_usage_;
    static std::map<std::string, double> coverage_data_;
};

} // namespace testing
} // namespace therapy_device
