/**
 * @file d435_camera.h
 * @brief Intel RealSense D435 camera implementation
 * 
 * Implementation of ICamera interface for Intel RealSense D435 camera.
 * This is the development platform camera that will be migrated to
 * ORBBEC Femto Mega for production.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-HW-D435-001, REQ-HW-D435-002, REQ-HW-D435-003
 */

#pragma once

#include "icamera.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>

#ifdef THERAPY_DEVICE_HAS_REALSENSE
#include <librealsense2/rs.hpp>
#endif

namespace therapy_device {
namespace hardware {

/**
 * @brief Intel RealSense D435 camera implementation
 * 
 * This class implements the ICamera interface for Intel RealSense D435
 * depth camera. It provides thread-safe access to RGB and depth streams
 * with medical device compliance features.
 * 
 * Key Features:
 * - Hardware timestamp synchronization
 * - Automatic error recovery
 * - Performance monitoring
 * - Data validation and integrity checks
 * - Medical device safety compliance
 * 
 * Thread Safety: All public methods are thread-safe.
 * Memory Management: Uses RAII for automatic resource management.
 * Error Handling: Comprehensive error codes and recovery mechanisms.
 * 
 * Traceability: REQ-HW-D435-001 (D435 implementation)
 */
class D435Camera : public ICamera {
public:
    /**
     * @brief Constructor
     * 
     * Initializes the D435 camera instance with default settings.
     * Does not establish hardware connection.
     */
    D435Camera();

    /**
     * @brief Destructor
     * 
     * Ensures proper cleanup and disconnection from hardware.
     */
    virtual ~D435Camera();

    // ICamera interface implementation
    CameraError connect() override;
    CameraError disconnect() override;
    CameraError startCapture(const CameraConfig& config) override;
    CameraError stopCapture() override;
    CameraError getDepthFrame(DepthFrame& frame) override;
    CameraError getRGBFrame(RGBFrame& frame) override;
    CameraError getSynchronizedFrames(RGBFrame& rgb_frame, DepthFrame& depth_frame) override;
    CameraCapabilities getCapabilities() const override;
    CameraStatus getStatus() const override;
    std::pair<CameraError, std::string> getLastError() const override;
    void setFrameCallback(FrameCallback callback) override;
    void setErrorCallback(ErrorCallback callback) override;
    void setStatusCallback(StatusCallback callback) override;
    CameraError calibrate() override;
    CameraError validate() override;
    
    struct {
        float current_fps;
        float average_latency_ms;
        int dropped_frames;
        float cpu_usage_percent;
        float memory_usage_mb;
        float temperature_celsius;
    } getPerformanceMetrics() const override;

    /**
     * @brief D435 specific methods
     */
    
    /**
     * @brief Get RealSense device information
     * @return Device information string
     */
    std::string getDeviceInfo() const;
    
    /**
     * @brief Set advanced D435 options
     * @param option Option name
     * @param value Option value
     * @return CameraError::SUCCESS on success, error code otherwise
     */
    CameraError setAdvancedOption(const std::string& option, float value);
    
    /**
     * @brief Get advanced D435 option value
     * @param option Option name
     * @return Option value, or -1 on error
     */
    float getAdvancedOption(const std::string& option) const;
    
    /**
     * @brief Enable/disable laser projector
     * @param enable True to enable laser, false to disable
     * @return CameraError::SUCCESS on success, error code otherwise
     */
    CameraError setLaserPower(bool enable);
    
    /**
     * @brief Get current laser power status
     * @return True if laser is enabled, false otherwise
     */
    bool isLaserEnabled() const;

private:
    /**
     * @brief Internal state management
     */
    std::atomic<CameraStatus> status_;
    std::atomic<CameraError> last_error_code_;
    std::string last_error_message_;
    mutable std::mutex error_mutex_;
    
    /**
     * @brief Configuration and capabilities
     */
    CameraConfig current_config_;
    CameraCapabilities capabilities_;
    bool capabilities_initialized_;
    
    /**
     * @brief Callback functions
     */
    FrameCallback frame_callback_;
    ErrorCallback error_callback_;
    StatusCallback status_callback_;
    std::mutex callback_mutex_;
    
    /**
     * @brief Threading and synchronization
     */
    std::unique_ptr<std::thread> capture_thread_;
    std::atomic<bool> capture_running_;
    std::condition_variable capture_cv_;
    std::mutex capture_mutex_;
    
    /**
     * @brief Frame management
     */
    struct FrameData {
        RGBFrame rgb;
        DepthFrame depth;
        std::chrono::high_resolution_clock::time_point timestamp;
        bool is_valid;
    };
    
    std::queue<FrameData> frame_buffer_;
    std::mutex frame_mutex_;
    std::condition_variable frame_cv_;
    static const size_t MAX_FRAME_BUFFER_SIZE = 10;
    
    /**
     * @brief Performance monitoring
     */
    struct PerformanceData {
        std::atomic<float> current_fps{0.0f};
        std::atomic<float> average_latency_ms{0.0f};
        std::atomic<int> dropped_frames{0};
        std::atomic<float> cpu_usage_percent{0.0f};
        std::atomic<float> memory_usage_mb{0.0f};
        std::atomic<float> temperature_celsius{0.0f};
        
        // Internal counters
        std::atomic<uint64_t> total_frames{0};
        std::atomic<uint64_t> successful_frames{0};
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point last_frame_time;
    } performance_;

#ifdef THERAPY_DEVICE_HAS_REALSENSE
    /**
     * @brief RealSense SDK objects
     */
    rs2::context rs_context_;
    rs2::device rs_device_;
    rs2::pipeline rs_pipeline_;
    rs2::config rs_config_;
    rs2::pipeline_profile rs_profile_;
    
    /**
     * @brief Stream profiles
     */
    rs2::stream_profile color_profile_;
    rs2::stream_profile depth_profile_;
    rs2::stream_profile infrared_profile_;
    
    /**
     * @brief Sensors and options
     */
    rs2::sensor color_sensor_;
    rs2::sensor depth_sensor_;
    rs2::depth_stereo_sensor stereo_sensor_;
    
    /**
     * @brief Processing blocks
     */
    rs2::align align_to_color_;
    rs2::decimation_filter decimation_filter_;
    rs2::threshold_filter threshold_filter_;
    rs2::spatial_filter spatial_filter_;
    rs2::temporal_filter temporal_filter_;
    rs2::hole_filling_filter hole_filling_filter_;
    rs2::colorizer colorizer_;
    
    bool filters_enabled_;
#endif

    /**
     * @brief Private helper methods
     */
    
    /**
     * @brief Initialize capabilities from hardware
     */
    void initializeCapabilities();
    
    /**
     * @brief Configure RealSense pipeline
     * @param config Camera configuration
     * @return CameraError::SUCCESS on success, error code otherwise
     */
    CameraError configurePipeline(const CameraConfig& config);
    
    /**
     * @brief Main capture thread function
     */
    void captureThreadFunction();
    
    /**
     * @brief Process RealSense frameset
     * @param frameset RealSense frameset
     * @return Processed frame data
     */
    FrameData processFrameset(const rs2::frameset& frameset);
    
    /**
     * @brief Convert RealSense color frame to RGB frame
     * @param color_frame RealSense color frame
     * @return RGB frame structure
     */
    RGBFrame convertColorFrame(const rs2::video_frame& color_frame);
    
    /**
     * @brief Convert RealSense depth frame to depth frame
     * @param depth_frame RealSense depth frame
     * @return Depth frame structure
     */
    DepthFrame convertDepthFrame(const rs2::depth_frame& depth_frame);
    
    /**
     * @brief Update performance metrics
     */
    void updatePerformanceMetrics();
    
    /**
     * @brief Validate frame data integrity
     * @param frame_data Frame data to validate
     * @return True if valid, false otherwise
     */
    bool validateFrameData(const FrameData& frame_data) const;
    
    /**
     * @brief Calculate frame checksum
     * @param data Frame data
     * @return CRC32 checksum
     */
    uint32_t calculateChecksum(const std::vector<uint8_t>& data) const;
    uint32_t calculateChecksum(const std::vector<uint16_t>& data) const;
    
    /**
     * @brief Handle internal error
     * @param error Error code
     * @param message Error message
     */
    void handleError(CameraError error, const std::string& message);
    
    /**
     * @brief Update camera status
     * @param new_status New status
     * @param message Status message
     */
    void updateStatus(CameraStatus new_status, const std::string& message = "");
    
    /**
     * @brief Check hardware temperature
     * @return Temperature in Celsius, or -1 on error
     */
    float checkTemperature() const;
    
    /**
     * @brief Perform self-diagnostic
     * @return CameraError::SUCCESS if all checks pass, error code otherwise
     */
    CameraError performSelfDiagnostic();
    
    /**
     * @brief Recovery from error state
     * @return CameraError::SUCCESS if recovery successful, error code otherwise
     */
    CameraError attemptRecovery();
    
    /**
     * @brief Safety monitoring
     * @return True if all safety checks pass, false otherwise
     */
    bool performSafetyChecks() const;
};

/**
 * @brief D435 Camera Factory
 * 
 * Factory implementation for creating D435 camera instances.
 */
class D435CameraFactory : public ICameraFactory {
public:
    std::unique_ptr<ICamera> createCamera(const std::string& camera_type) override;
    std::vector<std::string> getAvailableCameraTypes() const override;
    std::vector<std::string> detectConnectedCameras() const override;
    
private:
    /**
     * @brief Check if D435 camera is available
     * @return True if D435 is detected, false otherwise
     */
    bool isD435Available() const;
};

} // namespace hardware
} // namespace therapy_device
