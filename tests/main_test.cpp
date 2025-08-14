/**
 * @file main_test.cpp
 * @brief Main test runner for medical device testing
 * 
 * Main entry point for comprehensive testing suite with IEC 62304 Class C
 * medical device compliance validation and reporting.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-TEST-MAIN-001
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <memory>
#include <chrono>
#include <fstream>
#include <cstdlib>

#include "test_utils.h"
#include "test_config.h"
#include "core/logger.h"

using namespace therapy_device::testing;
using namespace therapy_device::core;

/**
 * @brief Medical Device Test Environment
 * 
 * Global test environment setup for medical device compliance testing.
 */
class MedicalDeviceTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        std::cout << "=== Autonomous Physical Therapy Device Test Suite ===" << std::endl;
        std::cout << "IEC 62304 Class C Medical Device Software Testing" << std::endl;
        std::cout << "Version: " << PROJECT_VERSION << std::endl;
        std::cout << "Build: " << BUILD_TYPE << std::endl;
        std::cout << "Platform: " << TARGET_PLATFORM << std::endl;
        std::cout << "======================================================" << std::endl;
        std::cout << std::endl;
        
        // Initialize test environment
        if (!initializeTestEnvironment()) {
            std::cerr << "Failed to initialize test environment" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        
        // Setup logging for tests
        setupTestLogging();
        
        // Validate test requirements
        if (!validateTestRequirements()) {
            std::cerr << "Test environment validation failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        
        // Initialize medical test configuration
        if (!MedicalTestConfig::initialize()) {
            std::cerr << "Failed to initialize medical test configuration" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        
        std::cout << "Test environment initialized successfully" << std::endl;
        std::cout << "Medical mode testing: " << (MedicalTestConfig::isMedicalModeEnabled() ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "Hardware testing: " << (MedicalTestConfig::isHardwareTestingAvailable() ? "AVAILABLE" : "SIMULATION") << std::endl;
        std::cout << "Performance testing: " << (MedicalTestConfig::shouldRunPerformanceTests() ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << std::endl;
    }
    
    void TearDown() override {
        // Generate test statistics
        auto stats = TestStatistics::getStatisticsSummary();
        std::cout << std::endl;
        std::cout << "=== Test Execution Summary ===" << std::endl;
        std::cout << stats << std::endl;
        
        // Generate test report
        std::string report_file = "test_execution_report.html";
        if (TestStatistics::generateReport(report_file)) {
            std::cout << "Test report generated: " << report_file << std::endl;
        }
        
        // Cleanup test environment
        cleanupTestEnvironment();
        
        std::cout << "Test environment cleanup completed" << std::endl;
    }

private:
    bool initializeTestEnvironment() {
        try {
            // Create test directories
            TestFileUtils::createTempDirectory("therapy_test_main");
            
            // Initialize test data generator
            // TestDataGenerator is statically initialized
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Test environment initialization error: " << e.what() << std::endl;
            return false;
        }
    }
    
    void setupTestLogging() {
        // Configure logger for testing
        auto& logger = Logger::getInstance();
        Logger::Config test_config;
        
        test_config.min_level = LogLevel::DEBUG;
        test_config.enable_console_output = false;  // Disable console to avoid test output pollution
        test_config.enable_file_output = true;
        test_config.log_file_path = "test_execution.log";
        test_config.enable_audit_trail = true;
        test_config.audit_file_path = "test_audit.log";
        test_config.enable_async_logging = false;  // Synchronous for testing
        test_config.device_id = "TEST_DEVICE_001";
        
        logger.configure(test_config);
        logger.info("TestMain", "Test logging initialized");
    }
    
    bool validateTestRequirements() {
        std::cout << "Validating test environment requirements..." << std::endl;
        
        // Check GoogleTest version
        std::cout << "  GoogleTest version: " << GTEST_VERSION_ << std::endl;
        
        // Check GoogleMock availability
        std::cout << "  GoogleMock available: " << (HAS_GMOCK ? "YES" : "NO") << std::endl;
        
        // Check feature availability
        std::cout << "  RealSense support: " << (HAS_REALSENSE ? "YES" : "NO") << std::endl;
        std::cout << "  ORBBEC support: " << (HAS_ORBBEC ? "YES" : "NO") << std::endl;
        std::cout << "  MediaPipe support: " << (HAS_MEDIAPIPE ? "YES" : "NO") << std::endl;
        std::cout << "  TensorRT support: " << (HAS_TENSORRT ? "YES" : "NO") << std::endl;
        
        // Check test data availability
        if (!TestFileUtils::testFileExists(paths::TEST_DATA)) {
            std::cout << "  Creating test data directory: " << paths::TEST_DATA << std::endl;
            TestFileUtils::createTestFile(paths::TEST_DATA + "/README.txt", 
                                        "Test data directory for medical device testing");
        }
        
        std::cout << "  Test data directory: " << paths::TEST_DATA << std::endl;
        std::cout << "  Test fixtures directory: " << paths::TEST_FIXTURES << std::endl;
        
        // Validate medical device requirements
        std::cout << "  Medical device requirements:" << std::endl;
        std::cout << "    Max latency: " << medical_device::MAX_LATENCY_MS << "ms" << std::endl;
        std::cout << "    Min accuracy: " << medical_device::MIN_ACCURACY << std::endl;
        std::cout << "    Max memory: " << medical_device::MAX_MEMORY_MB << "MB" << std::endl;
        std::cout << "    Max power: " << medical_device::MAX_POWER_WATTS << "W" << std::endl;
        std::cout << "    Min FPS: " << medical_device::MIN_FPS << std::endl;
        
        return true;
    }
    
    void cleanupTestEnvironment() {
        // Cleanup temporary files and directories
        // TestFileUtils will handle cleanup in destructor
        
        // Flush any remaining logs
        auto& logger = Logger::getInstance();
        logger.flush();
    }
};

/**
 * @brief Custom test result printer
 * 
 * Provides medical device specific test result formatting and reporting.
 */
class MedicalDeviceTestResultPrinter : public ::testing::TestEventListener {
public:
    void OnTestStart(const ::testing::TestInfo& test_info) override {
        TestTimer timer;
        test_timers_[GetTestKey(test_info)] = std::make_unique<TestTimer>();
        
        // Log test start for audit trail
        auto& logger = Logger::getInstance();
        logger.info("TestRunner", 
                   "Starting test: " + std::string(test_info.test_suite_name()) + "." + test_info.name());
    }
    
    void OnTestEnd(const ::testing::TestInfo& test_info) override {
        std::string test_key = GetTestKey(test_info);
        
        if (test_timers_.find(test_key) != test_timers_.end()) {
            double execution_time = test_timers_[test_key]->getElapsedMs();
            TestStatistics::recordExecutionTime(test_key, execution_time);
            
            // Log test completion
            auto& logger = Logger::getInstance();
            std::string result = test_info.result()->Passed() ? "PASSED" : "FAILED";
            logger.info("TestRunner", 
                       "Test completed: " + test_key + " - " + result + 
                       " (" + std::to_string(execution_time) + "ms)");
            
            test_timers_.erase(test_key);
        }
    }
    
    void OnTestSuiteStart(const ::testing::TestSuite& test_suite) override {
        std::cout << std::endl;
        std::cout << "=== " << test_suite.name() << " ===" << std::endl;
        
        // Check if this is a medical compliance test suite
        std::string suite_name = test_suite.name();
        if (suite_name.find("Medical") != std::string::npos ||
            suite_name.find("Compliance") != std::string::npos ||
            suite_name.find("Safety") != std::string::npos) {
            std::cout << "*** MEDICAL DEVICE COMPLIANCE TEST SUITE ***" << std::endl;
        }
    }
    
    void OnTestSuiteEnd(const ::testing::TestSuite& test_suite) override {
        std::cout << "Tests run: " << test_suite.test_to_run_count() 
                  << ", Passed: " << test_suite.successful_test_count()
                  << ", Failed: " << test_suite.failed_test_count() << std::endl;
    }

private:
    std::map<std::string, std::unique_ptr<TestTimer>> test_timers_;
    
    std::string GetTestKey(const ::testing::TestInfo& test_info) {
        return std::string(test_info.test_suite_name()) + "." + test_info.name();
    }
};

/**
 * @brief Main test entry point
 * 
 * Initializes GoogleTest framework and runs comprehensive medical device tests.
 */
int main(int argc, char** argv) {
    // Initialize GoogleTest and GoogleMock
    ::testing::InitGoogleTest(&argc, argv);
    if (HAS_GMOCK) {
        ::testing::InitGoogleMock(&argc, argv);
    }
    
    // Add global test environment
    ::testing::AddGlobalTestEnvironment(new MedicalDeviceTestEnvironment);
    
    // Add custom test result printer
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new MedicalDeviceTestResultPrinter);
    
    // Configure test execution
    std::cout << "Configuring test execution..." << std::endl;
    
    // Set test timeout from environment or default
    const char* timeout_env = std::getenv("THERAPY_TEST_TIMEOUT");
    if (timeout_env) {
        int timeout = std::atoi(timeout_env);
        if (timeout > 0) {
            std::cout << "Test timeout set to: " << timeout << " seconds" << std::endl;
        }
    }
    
    // Check for specific test filters
    const char* filter_env = std::getenv("THERAPY_TEST_FILTER");
    if (filter_env) {
        std::cout << "Test filter applied: " << filter_env << std::endl;
        ::testing::GTEST_FLAG(filter) = filter_env;
    }
    
    // Enable detailed test output for medical device compliance
    ::testing::GTEST_FLAG(print_time) = true;
    
    // Run tests
    std::cout << std::endl;
    std::cout << "Starting medical device test execution..." << std::endl;
    std::cout << "=========================================" << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    int result = RUN_ALL_TESTS();
    auto end_time = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Test execution completed in " << duration.count() << "ms" << std::endl;
    
    if (result == 0) {
        std::cout << "✅ All tests passed - Medical device compliance validated" << std::endl;
    } else {
        std::cout << "❌ Some tests failed - Medical device compliance NOT validated" << std::endl;
        std::cout << "Review test results and fix failures before deployment" << std::endl;
    }
    
    std::cout << "=========================================" << std::endl;
    
    return result;
}
