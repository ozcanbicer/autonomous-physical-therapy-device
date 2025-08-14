/**
 * @file mock_camera.h
 * @brief Mock camera implementation for testing
 * 
 * Comprehensive mock camera implementation for unit and integration testing
 * of medical device software without requiring physical hardware.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-TEST-MOCK-001, REQ-TEST-MOCK-002
 */

#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "hardware/icamera.h"
#include "test_utils.h"
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace therapy_device {
namespace testing {

/**
 * @brief Mock camera implementation using GoogleMock
 * 
 * Provides controllable mock camera for testing all camera interface
 * functionality without requiring physical hardware.
 */
class MockCamera : public hardware::ICamera {
public:
    MockCamera();
    virtual ~MockCamera();

    // Mock methods for all ICamera interface methods
    MOCK_METHOD(hardware::CameraError, connect, (), (override));
    MOCK_METHOD(hardware::CameraError, disconnect, (), (override));
    MOCK_METHOD(hardware::CameraError, startCapture, (const hardware::CameraConfig& config), (override));
    MOCK_METHOD(hardware::CameraError, stopCapture, (), (override));
    MOCK_METHOD(hardware::CameraError, getDepthFrame, (hardware::DepthFrame& frame), (override));
    MOCK_METHOD(hardware::CameraError, getRGBFrame, (hardware::RGBFrame& frame), (override));
    MOCK_METHOD(hardware::CameraError, getSynchronizedFrames, 
                (hardware::RGBFrame& rgb_frame, hardware::DepthFrame& depth_frame), (override));
    MOCK_METHOD(hardware::CameraCapabilities, getCapabilities, (), (const, override));
    MOCK_METHOD(hardware::CameraStatus, getStatus, (), (const, override));
    MOCK_METHOD((std::pair<hardware::CameraError, std::string>), getLastError, (), (const, override));
    MOCK_METHOD(void, setFrameCallback, (hardware::FrameCallback callback), (override));
    MOCK_METHOD(void, setErrorCallback, (hardware::ErrorCallback callback), (override));
    MOCK_METHOD(void, setStatusCallback, (hardware::StatusCallback callback), (override));
    MOCK_METHOD(hardware::CameraError, calibrate, (), (override));
    MOCK_METHOD(hardware::CameraError, validate, (), (override));
    
    // Performance metrics mock
    MOCK_METHOD((struct {
        float current_fps;
        float average_latency_ms;
        int dropped_frames;
        float cpu_usage_percent;
        float memory_usage_mb;
        float temperature_celsius;
    }), getPerformanceMetrics, (), (const, override));

    /**
     * @brief Setup default mock behavior
     * 
     * Configures realistic default behavior for all mock methods.
     */
    void setupDefaultBehavior();
    
    /**
     * @brief Setup error behavior
     * @param error_code Error code to return
     * @param error_message Error message
     */
    void setupErrorBehavior(hardware::CameraError error_code, const std::string& error_message);
    
    /**
     * @brief Setup frame generation
     * @param enable_generation Enable automatic frame generation
     * @param fps Frames per second for generation
     */
    void setupFrameGeneration(bool enable_generation, int fps = 30);
    
    /**
     * @brief Simulate camera connection
     * @param success True if connection should succeed
     * @param delay_ms Connection delay in milliseconds
     */
    void simulateConnection(bool success, int delay_ms = 100);
    
    /**
     * @brief Simulate camera failure
     * @param failure_type Type of failure to simulate
     * @param recovery_time_ms Time until recovery in milliseconds
     */
    void simulateFailure(const std::string& failure_type, int recovery_time_ms = 5000);
    
    /**
     * @brief Set performance characteristics
     * @param fps Frame rate
     * @param latency_ms Processing latency
     * @param cpu_usage CPU usage percentage
     */
    void setPerformanceCharacteristics(float fps, float latency_ms, float cpu_usage);

private:
    // Internal state
    std::atomic<hardware::CameraStatus> status_;
    std::atomic<hardware::CameraError> last_error_;
    std::string last_error_message_;
    hardware::CameraConfig current_config_;
    hardware::CameraCapabilities capabilities_;
    
    // Frame generation
    std::atomic<bool> frame_generation_enabled_;
    std::atomic<int> target_fps_;
    std::unique_ptr<std::thread> frame_generation_thread_;
    std::atomic<bool> thread_running_;
    
    // Frame callbacks
    hardware::FrameCallback frame_callback_;
    hardware::ErrorCallback error_callback_;
    hardware::StatusCallback status_callback_;
    std::mutex callback_mutex_;
    
    // Performance metrics
    struct {
        std::atomic<float> current_fps{30.0f};
        std::atomic<float> average_latency_ms{10.0f};
        std::atomic<int> dropped_frames{0};
        std::atomic<float> cpu_usage_percent{15.0f};
        std::atomic<float> memory_usage_mb{100.0f};
        std::atomic<float> temperature_celsius{45.0f};
    } performance_metrics_;
    
    // Frame generation thread
    void frameGenerationThread();
    
    // Helper methods
    void updateStatus(hardware::CameraStatus new_status);
    void triggerError(hardware::CameraError error, const std::string& message);
};

/**
 * @brief Realistic camera simulator
 * 
 * Provides realistic camera behavior simulation for integration testing.
 */
class RealisticCameraSimulator : public hardware::ICamera {
public:
    /**
     * @brief Constructor
     * @param camera_type Type of camera to simulate ("d435", "femto_mega")
     */
    explicit RealisticCameraSimulator(const std::string& camera_type = "d435");
    
    virtual ~RealisticCameraSimulator();

    // ICamera interface implementation with realistic behavior
    hardware::CameraError connect() override;
    hardware::CameraError disconnect() override;
    hardware::CameraError startCapture(const hardware::CameraConfig& config) override;
    hardware::CameraError stopCapture() override;
    hardware::CameraError getDepthFrame(hardware::DepthFrame& frame) override;
    hardware::CameraError getRGBFrame(hardware::RGBFrame& frame) override;
    hardware::CameraError getSynchronizedFrames(hardware::RGBFrame& rgb_frame, 
                                               hardware::DepthFrame& depth_frame) override;
    hardware::CameraCapabilities getCapabilities() const override;
    hardware::CameraStatus getStatus() const override;
    std::pair<hardware::CameraError, std::string> getLastError() const override;
    void setFrameCallback(hardware::FrameCallback callback) override;
    void setErrorCallback(hardware::ErrorCallback callback) override;
    void setStatusCallback(hardware::StatusCallback callback) override;
    hardware::CameraError calibrate() override;
    hardware::CameraError validate() override;
    
    struct {
        float current_fps;
        float average_latency_ms;
        int dropped_frames;
        float cpu_usage_percent;
        float memory_usage_mb;
        float temperature_celsius;
    } getPerformanceMetrics() const override;

    /**
     * @brief Configuration methods
     */
    
    /**
     * @brief Set simulation parameters
     * @param params Simulation parameters
     */
    struct SimulationParams {
        bool simulate_realistic_delays = true;
        bool simulate_occasional_errors = false;
        double error_probability = 0.01;  // 1% error rate
        bool simulate_thermal_effects = true;
        bool simulate_performance_variation = true;
        int warmup_time_ms = 2000;
        double noise_level = 0.02;  // 2% noise
    };
    
    void setSimulationParams(const SimulationParams& params);
    
    /**
     * @brief Inject specific error for testing
     * @param error_code Error to inject
     * @param message Error message
     * @param duration_ms Duration of error in milliseconds
     */
    void injectError(hardware::CameraError error_code, const std::string& message, int duration_ms);
    
    /**
     * @brief Set thermal simulation
     * @param enable Enable thermal simulation
     * @param base_temp Base temperature in Celsius
     * @param max_temp Maximum temperature in Celsius
     */
    void setThermalSimulation(bool enable, float base_temp = 35.0f, float max_temp = 70.0f);

private:
    std::string camera_type_;
    std::atomic<hardware::CameraStatus> status_;
    std::atomic<hardware::CameraError> last_error_;
    std::string last_error_message_;
    mutable std::mutex error_mutex_;
    
    hardware::CameraConfig current_config_;
    hardware::CameraCapabilities capabilities_;
    SimulationParams simulation_params_;
    
    // Frame generation and callbacks
    hardware::FrameCallback frame_callback_;
    hardware::ErrorCallback error_callback_;
    hardware::StatusCallback status_callback_;
    std::mutex callback_mutex_;
    
    // Threading for realistic behavior
    std::unique_ptr<std::thread> simulation_thread_;
    std::atomic<bool> simulation_running_;
    std::condition_variable simulation_cv_;
    std::mutex simulation_mutex_;
    
    // Frame buffers
    std::queue<hardware::RGBFrame> rgb_buffer_;
    std::queue<hardware::DepthFrame> depth_buffer_;
    std::mutex frame_mutex_;
    static const size_t MAX_BUFFER_SIZE = 10;
    
    // Performance simulation
    struct {
        std::atomic<float> current_fps{0.0f};
        std::atomic<float> average_latency_ms{0.0f};
        std::atomic<int> dropped_frames{0};
        std::atomic<float> cpu_usage_percent{0.0f};
        std::atomic<float> memory_usage_mb{0.0f};
        std::atomic<float> temperature_celsius{35.0f};
        
        std::atomic<uint64_t> total_frames{0};
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point last_frame_time;
    } performance_;
    
    // Thermal simulation
    bool thermal_simulation_enabled_;
    float base_temperature_;
    float max_temperature_;
    std::chrono::high_resolution_clock::time_point startup_time_;
    
    // Error injection
    struct InjectedError {
        hardware::CameraError error_code;
        std::string message;
        std::chrono::high_resolution_clock::time_point expiry_time;
        bool active;
    } injected_error_;
    std::mutex injection_mutex_;
    
    // Private helper methods
    void initializeCapabilities();
    void simulationThreadFunction();
    void generateFrames();
    void updatePerformanceMetrics();
    void simulateRealisticDelays();
    bool shouldSimulateError();
    void updateThermalState();
    hardware::RGBFrame generateRealisticRGBFrame();
    hardware::DepthFrame generateRealisticDepthFrame();
    uint32_t calculateFrameChecksum(const std::vector<uint8_t>& data);
    uint32_t calculateFrameChecksum(const std::vector<uint16_t>& data);
    void handleError(hardware::CameraError error, const std::string& message);
    void updateStatus(hardware::CameraStatus new_status);
};

/**
 * @brief Camera test fixture
 * 
 * Base test fixture for camera-related tests with common setup and teardown.
 */
class CameraTestFixture : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    
    // Test utilities
    std::unique_ptr<MockCamera> mock_camera_;
    std::unique_ptr<RealisticCameraSimulator> realistic_camera_;
    TestTimer timer_;
    
    // Common test data
    hardware::CameraConfig test_config_;
    hardware::RGBFrame sample_rgb_frame_;
    hardware::DepthFrame sample_depth_frame_;
    
    // Helper methods
    void setupMockCamera();
    void setupRealisticCamera(const std::string& camera_type = "d435");
    void verifyFrameQuality(const hardware::RGBFrame& frame);
    void verifyFrameQuality(const hardware::DepthFrame& frame);
    void verifyPerformanceRequirements();
};

/**
 * @brief Parameterized camera test fixture
 * 
 * Test fixture for running the same tests with different camera types.
 */
class ParameterizedCameraTest : public ::testing::TestWithParam<std::string> {
protected:
    void SetUp() override;
    void TearDown() override;
    
    std::unique_ptr<RealisticCameraSimulator> camera_;
    std::string camera_type_;
};

/**
 * @brief Camera performance test fixture
 * 
 * Specialized fixture for performance and stress testing of cameras.
 */
class CameraPerformanceTest : public CameraTestFixture {
protected:
    void SetUp() override;
    void TearDown() override;
    
    // Performance test utilities
    void runLatencyTest(int num_frames = 1000);
    void runThroughputTest(int duration_seconds = 60);
    void runStressTest(int duration_seconds = 300);
    void runMemoryLeakTest(int iterations = 10000);
    
    // Performance validation
    void validateLatencyRequirements(double average_latency_ms);
    void validateThroughputRequirements(double fps);
    void validateMemoryRequirements(double memory_usage_mb);
    
private:
    std::vector<double> latency_measurements_;
    std::vector<double> throughput_measurements_;
    std::vector<double> memory_measurements_;
};

} // namespace testing
} // namespace therapy_device
