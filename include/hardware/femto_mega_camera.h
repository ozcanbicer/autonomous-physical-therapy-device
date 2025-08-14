/**
 * @file femto_mega_camera.h
 * @brief ORBBEC Femto Mega camera implementation
 * 
 * Implementation of ICamera interface for ORBBEC Femto Mega camera.
 * This is the production platform camera optimized for Jetson Orin NX
 * with enhanced performance and medical device compliance.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-HW-FEMTO-001, REQ-HW-FEMTO-002, REQ-HW-FEMTO-003
 */

#pragma once

#include "icamera.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <memory>

#ifdef THERAPY_DEVICE_HAS_ORBBEC
#include "OrbbecSDK.hpp"
#endif

namespace therapy_device {
namespace hardware {

/**
 * @brief ORBBEC Femto Mega camera implementation
 * 
 * This class implements the ICamera interface for ORBBEC Femto Mega
 * depth camera. Optimized for production use on Jetson Orin NX with
 * enhanced performance and medical device compliance features.
 * 
 * Key Features:
 * - Hardware-accelerated processing on Jetson
 * - Advanced depth filtering and noise reduction
 * - Multi-stream synchronization
 * - Enhanced power management
 * - Medical grade accuracy and reliability
 * - CUDA acceleration support
 * 
 * Production Advantages over D435:
 * - Higher depth accuracy (±1mm vs ±2mm)
 * - Better performance in low light
 * - Lower power consumption
 * - Hardware sync capabilities
 * - Medical device certification ready
 * 
 * Thread Safety: All public methods are thread-safe.
 * Memory Management: Optimized for embedded systems.
 * Error Handling: Enhanced recovery mechanisms.
 * 
 * Traceability: REQ-HW-FEMTO-001 (Femto Mega implementation)
 */
class FemtoMegaCamera : public ICamera {
public:
    /**
     * @brief Constructor
     * 
     * Initializes the Femto Mega camera instance with production settings.
     * Optimized for Jetson Orin NX platform.
     */
    FemtoMegaCamera();

    /**
     * @brief Destructor
     * 
     * Ensures proper cleanup and power management.
     */
    virtual ~FemtoMegaCamera();

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
     * @brief Femto Mega specific methods
     */
    
    /**
     * @brief Enable hardware acceleration
     * @param enable True to enable CUDA acceleration
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Enables CUDA-based hardware acceleration for depth processing
     * on Jetson Orin NX. Significantly improves performance.
     */
    CameraError enableHardwareAcceleration(bool enable);
    
    /**
     * @brief Set advanced depth filtering
     * @param filter_level Filter level (0-5, 0=disabled, 5=maximum)
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Configures advanced depth filtering algorithms for improved
     * accuracy in medical applications.
     */
    CameraError setDepthFiltering(int filter_level);
    
    /**
     * @brief Configure multi-exposure HDR
     * @param enable True to enable HDR mode
     * @param exposure_times Vector of exposure times in microseconds
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Enables High Dynamic Range imaging for better performance
     * in varying lighting conditions.
     */
    CameraError configureHDR(bool enable, const std::vector<int>& exposure_times = {});
    
    /**
     * @brief Set power management mode
     * @param mode Power mode ("performance", "balanced", "power_save")
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Configures power management for optimal performance vs
     * power consumption balance on Jetson platform.
     */
    CameraError setPowerMode(const std::string& mode);
    
    /**
     * @brief Get thermal status
     * @return Thermal status information
     * 
     * Returns detailed thermal information for monitoring
     * and safety compliance.
     */
    struct ThermalStatus {
        float sensor_temperature;
        float housing_temperature;
        float ambient_temperature;
        bool thermal_warning;
        bool thermal_shutdown_imminent;
        int thermal_throttling_level;
    } getThermalStatus() const;
    
    /**
     * @brief Perform factory calibration
     * @param calibration_target Target type ("checkerboard", "sphere")
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Performs factory-grade calibration for medical device accuracy.
     * Required for IEC 62304 compliance.
     */
    CameraError performFactoryCalibration(const std::string& calibration_target);
    
    /**
     * @brief Get calibration certificate
     * @return Calibration certificate data
     * 
     * Returns digital calibration certificate for regulatory compliance.
     */
    std::string getCalibrationCertificate() const;

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
     * @brief Production-specific settings
     */
    bool hardware_acceleration_enabled_;
    int depth_filter_level_;
    bool hdr_enabled_;
    std::string power_mode_;
    
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
    std::unique_ptr<std::thread> processing_thread_;
    std::atomic<bool> capture_running_;
    std::atomic<bool> processing_running_;
    std::condition_variable capture_cv_;
    std::condition_variable processing_cv_;
    std::mutex capture_mutex_;
    std::mutex processing_mutex_;
    
    /**
     * @brief Advanced frame management
     */
    struct FrameData {
        RGBFrame rgb;
        DepthFrame depth;
        std::chrono::high_resolution_clock::time_point timestamp;
        bool is_valid;
        float confidence_score;
        uint32_t sequence_number;
    };
    
    std::queue<FrameData> raw_frame_buffer_;
    std::queue<FrameData> processed_frame_buffer_;
    std::mutex raw_frame_mutex_;
    std::mutex processed_frame_mutex_;
    std::condition_variable raw_frame_cv_;
    std::condition_variable processed_frame_cv_;
    static const size_t MAX_FRAME_BUFFER_SIZE = 15;
    
    /**
     * @brief Enhanced performance monitoring
     */
    struct PerformanceData {
        std::atomic<float> current_fps{0.0f};
        std::atomic<float> average_latency_ms{0.0f};
        std::atomic<int> dropped_frames{0};
        std::atomic<float> cpu_usage_percent{0.0f};
        std::atomic<float> gpu_usage_percent{0.0f};
        std::atomic<float> memory_usage_mb{0.0f};
        std::atomic<float> gpu_memory_usage_mb{0.0f};
        std::atomic<float> temperature_celsius{0.0f};
        std::atomic<float> power_consumption_watts{0.0f};
        
        // Production metrics
        std::atomic<uint64_t> total_frames{0};
        std::atomic<uint64_t> successful_frames{0};
        std::atomic<uint64_t> processed_frames{0};
        std::atomic<float> processing_efficiency{0.0f};
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point last_frame_time;
    } performance_;
    
    /**
     * @brief Thermal management
     */
    struct ThermalData {
        std::atomic<float> sensor_temp{0.0f};
        std::atomic<float> housing_temp{0.0f};
        std::atomic<float> ambient_temp{0.0f};
        std::atomic<bool> thermal_warning{false};
        std::atomic<bool> shutdown_imminent{false};
        std::atomic<int> throttling_level{0};
    } thermal_;

#ifdef THERAPY_DEVICE_HAS_ORBBEC
    /**
     * @brief ORBBEC SDK objects
     */
    std::shared_ptr<ob::Context> ob_context_;
    std::shared_ptr<ob::Device> ob_device_;
    std::shared_ptr<ob::Pipeline> ob_pipeline_;
    std::shared_ptr<ob::Config> ob_config_;
    
    /**
     * @brief Stream profiles
     */
    std::shared_ptr<ob::VideoStreamProfile> color_profile_;
    std::shared_ptr<ob::VideoStreamProfile> depth_profile_;
    std::shared_ptr<ob::VideoStreamProfile> infrared_profile_;
    
    /**
     * @brief Sensors
     */
    std::shared_ptr<ob::Sensor> color_sensor_;
    std::shared_ptr<ob::Sensor> depth_sensor_;
    std::shared_ptr<ob::Sensor> infrared_sensor_;
    
    /**
     * @brief Processing filters
     */
    std::shared_ptr<ob::PointCloudFilter> pointcloud_filter_;
    std::shared_ptr<ob::FormatConvertFilter> format_converter_;
    std::shared_ptr<ob::Align> align_filter_;
    
    // Advanced filters for production
    std::shared_ptr<ob::NoiseRemovalFilter> noise_removal_filter_;
    std::shared_ptr<ob::EdgePreservingFilter> edge_preserving_filter_;
    std::shared_ptr<ob::TemporalFilter> temporal_filter_;
    std::shared_ptr<ob::ThresholdFilter> threshold_filter_;
    
    bool advanced_filters_enabled_;
#endif

    /**
     * @brief Private helper methods
     */
    
    /**
     * @brief Initialize production capabilities
     */
    void initializeCapabilities();
    
    /**
     * @brief Configure ORBBEC pipeline for production
     * @param config Camera configuration
     * @return CameraError::SUCCESS on success, error code otherwise
     */
    CameraError configurePipeline(const CameraConfig& config);
    
    /**
     * @brief Main capture thread function
     */
    void captureThreadFunction();
    
    /**
     * @brief Main processing thread function
     */
    void processingThreadFunction();
    
    /**
     * @brief Process ORBBEC frameset with hardware acceleration
     * @param frameset ORBBEC frameset
     * @return Processed frame data
     */
    FrameData processFrameset(const ob::FrameSet& frameset);
    
    /**
     * @brief Apply advanced depth filtering
     * @param depth_frame Input depth frame
     * @return Filtered depth frame
     */
    DepthFrame applyAdvancedFiltering(const DepthFrame& depth_frame);
    
    /**
     * @brief Convert ORBBEC color frame to RGB frame
     * @param color_frame ORBBEC color frame
     * @return RGB frame structure
     */
    RGBFrame convertColorFrame(const ob::ColorFrame& color_frame);
    
    /**
     * @brief Convert ORBBEC depth frame to depth frame
     * @param depth_frame ORBBEC depth frame
     * @return Depth frame structure
     */
    DepthFrame convertDepthFrame(const ob::DepthFrame& depth_frame);
    
    /**
     * @brief Update performance metrics with GPU monitoring
     */
    void updatePerformanceMetrics();
    
    /**
     * @brief Monitor thermal status
     */
    void updateThermalStatus();
    
    /**
     * @brief Validate frame data with enhanced checks
     * @param frame_data Frame data to validate
     * @return True if valid, false otherwise
     */
    bool validateFrameData(const FrameData& frame_data) const;
    
    /**
     * @brief Calculate enhanced checksum with error correction
     * @param data Frame data
     * @return CRC32 checksum with ECC
     */
    uint32_t calculateEnhancedChecksum(const std::vector<uint8_t>& data) const;
    uint32_t calculateEnhancedChecksum(const std::vector<uint16_t>& data) const;
    
    /**
     * @brief Handle internal error with recovery
     * @param error Error code
     * @param message Error message
     */
    void handleError(CameraError error, const std::string& message);
    
    /**
     * @brief Update camera status with telemetry
     * @param new_status New status
     * @param message Status message
     */
    void updateStatus(CameraStatus new_status, const std::string& message = "");
    
    /**
     * @brief Comprehensive thermal monitoring
     * @return Temperature readings and status
     */
    ThermalStatus checkThermalStatus() const;
    
    /**
     * @brief Perform comprehensive self-diagnostic
     * @return CameraError::SUCCESS if all checks pass, error code otherwise
     */
    CameraError performSelfDiagnostic();
    
    /**
     * @brief Advanced recovery from error state
     * @return CameraError::SUCCESS if recovery successful, error code otherwise
     */
    CameraError attemptRecovery();
    
    /**
     * @brief Medical device safety monitoring
     * @return True if all safety checks pass, false otherwise
     */
    bool performSafetyChecks() const;
    
    /**
     * @brief Power management optimization
     */
    void optimizePowerConsumption();
    
    /**
     * @brief GPU memory management
     */
    void manageGPUMemory();
    
    /**
     * @brief Quality assurance validation
     * @return True if QA checks pass, false otherwise
     */
    bool performQualityAssurance() const;
};

/**
 * @brief Femto Mega Camera Factory
 * 
 * Factory implementation for creating Femto Mega camera instances.
 * Optimized for production deployment.
 */
class FemtoMegaCameraFactory : public ICameraFactory {
public:
    std::unique_ptr<ICamera> createCamera(const std::string& camera_type) override;
    std::vector<std::string> getAvailableCameraTypes() const override;
    std::vector<std::string> detectConnectedCameras() const override;
    
    /**
     * @brief Production-specific methods
     */
    
    /**
     * @brief Validate production readiness
     * @return True if ready for production use
     */
    bool validateProductionReadiness() const;
    
    /**
     * @brief Get factory calibration status
     * @return Calibration status information
     */
    std::string getFactoryCalibrationStatus() const;
    
private:
    /**
     * @brief Check if Femto Mega camera is available
     * @return True if Femto Mega is detected, false otherwise
     */
    bool isFemtoMegaAvailable() const;
    
    /**
     * @brief Validate hardware compatibility
     * @return True if hardware is compatible, false otherwise
     */
    bool validateHardwareCompatibility() const;
};

} // namespace hardware
} // namespace therapy_device
