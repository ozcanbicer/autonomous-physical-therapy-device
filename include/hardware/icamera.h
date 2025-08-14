/**
 * @file icamera.h
 * @brief Camera interface for hardware abstraction layer
 * 
 * This interface provides a unified API for different camera hardware,
 * enabling seamless migration between Intel RealSense D435 and ORBBEC Femto Mega.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-HW-001, REQ-HW-002, REQ-HW-003
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace therapy_device {
namespace hardware {

/**
 * @brief Camera capabilities structure
 * 
 * Contains information about camera hardware capabilities
 * for validation and configuration purposes.
 */
struct CameraCapabilities {
    std::string model_name;              ///< Camera model identifier
    std::string serial_number;           ///< Unique serial number
    std::string firmware_version;        ///< Firmware version
    
    // Resolution capabilities
    std::vector<std::pair<int, int>> supported_resolutions;  ///< Supported (width, height) pairs
    std::vector<int> supported_fps;      ///< Supported frame rates
    
    // Depth capabilities
    float min_depth_mm;                  ///< Minimum depth range in mm
    float max_depth_mm;                  ///< Maximum depth range in mm
    float depth_accuracy_mm;             ///< Depth accuracy in mm
    float depth_scale;                   ///< Depth scale factor
    
    // Color capabilities
    bool has_color_stream;               ///< Color stream availability
    bool has_infrared_stream;            ///< Infrared stream availability
    
    // Performance characteristics
    float max_frame_rate;                ///< Maximum achievable frame rate
    int power_consumption_mw;            ///< Power consumption in milliwatts
    
    // Medical device specific
    bool is_medical_grade;               ///< Medical grade certification
    std::string calibration_date;        ///< Last calibration date
    bool requires_recalibration;         ///< Calibration status
};

/**
 * @brief RGB frame data structure
 * 
 * Contains RGB image data with metadata for processing.
 * Memory layout is optimized for medical device requirements.
 */
struct RGBFrame {
    std::vector<uint8_t> data;           ///< RGB pixel data (BGR format)
    int width;                           ///< Frame width in pixels
    int height;                          ///< Frame height in pixels
    int channels;                        ///< Number of color channels (3 for RGB)
    int bytes_per_pixel;                 ///< Bytes per pixel (typically 3)
    
    std::chrono::high_resolution_clock::time_point timestamp;  ///< Capture timestamp
    uint64_t frame_number;               ///< Sequential frame number
    
    // Quality metrics
    float brightness;                    ///< Average brightness (0-255)
    float contrast;                      ///< Contrast measure
    bool is_valid;                       ///< Data validity flag
    
    // Medical device metadata
    std::string device_id;               ///< Source device identifier
    uint32_t checksum;                   ///< Data integrity checksum
};

/**
 * @brief Depth frame data structure
 * 
 * Contains depth image data with calibration information.
 * Critical for accurate pose estimation and safety validation.
 */
struct DepthFrame {
    std::vector<uint16_t> data;          ///< Depth values in millimeters
    int width;                           ///< Frame width in pixels
    int height;                          ///< Frame height in pixels
    float depth_scale;                   ///< Scale factor (mm per unit)
    
    std::chrono::high_resolution_clock::time_point timestamp;  ///< Capture timestamp
    uint64_t frame_number;               ///< Sequential frame number
    
    // Depth quality metrics
    float min_depth;                     ///< Minimum depth value in frame
    float max_depth;                     ///< Maximum depth value in frame
    float average_depth;                 ///< Average depth value
    int valid_pixels;                    ///< Number of valid depth pixels
    
    // Calibration data
    struct {
        float fx, fy;                    ///< Focal lengths
        float cx, cy;                    ///< Principal point
        std::vector<float> distortion;   ///< Distortion coefficients
    } intrinsics;
    
    bool is_valid;                       ///< Data validity flag
    std::string device_id;               ///< Source device identifier
    uint32_t checksum;                   ///< Data integrity checksum
};

/**
 * @brief Camera error codes
 * 
 * Standardized error codes for camera operations.
 * Used for IEC 62304 traceability and error handling.
 */
enum class CameraError {
    SUCCESS = 0,
    DEVICE_NOT_FOUND = 1001,
    CONNECTION_FAILED = 1002,
    INITIALIZATION_FAILED = 1003,
    CAPTURE_FAILED = 1004,
    INVALID_CONFIGURATION = 1005,
    HARDWARE_FAULT = 1006,
    FIRMWARE_ERROR = 1007,
    CALIBRATION_ERROR = 1008,
    TIMEOUT = 1009,
    INSUFFICIENT_POWER = 1010,
    TEMPERATURE_ERROR = 1011,
    MEMORY_ERROR = 1012,
    SAFETY_VIOLATION = 1013
};

/**
 * @brief Camera status enumeration
 * 
 * Represents the current operational status of the camera.
 */
enum class CameraStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    INITIALIZING,
    READY,
    CAPTURING,
    ERROR,
    FAULT
};

/**
 * @brief Camera configuration structure
 * 
 * Configuration parameters for camera initialization and operation.
 */
struct CameraConfig {
    // Resolution settings
    int width = 640;                     ///< Frame width
    int height = 480;                    ///< Frame height
    int fps = 30;                        ///< Target frame rate
    
    // Stream settings
    bool enable_color = true;            ///< Enable color stream
    bool enable_depth = true;            ///< Enable depth stream
    bool enable_infrared = false;        ///< Enable infrared stream
    
    // Quality settings
    int color_format = 0;                ///< Color format (0=BGR, 1=RGB)
    bool auto_exposure = true;           ///< Auto exposure control
    int exposure_value = 0;              ///< Manual exposure value
    bool auto_white_balance = true;      ///< Auto white balance
    
    // Performance settings
    bool enable_hardware_sync = true;    ///< Hardware timestamp sync
    int buffer_size = 5;                 ///< Frame buffer size
    int timeout_ms = 1000;               ///< Operation timeout
    
    // Medical device settings
    bool enable_validation = true;       ///< Enable data validation
    bool enable_checksums = true;        ///< Enable data integrity checks
    float min_confidence = 0.8f;         ///< Minimum data confidence
    
    // Safety settings
    float max_temperature = 70.0f;       ///< Maximum operating temperature
    int watchdog_timeout_ms = 5000;      ///< Watchdog timeout
    bool enable_safety_monitoring = true; ///< Safety monitoring
};

/**
 * @brief Camera event callback types
 * 
 * Callback function types for camera events and data.
 */
using FrameCallback = std::function<void(const RGBFrame&, const DepthFrame&)>;
using ErrorCallback = std::function<void(CameraError, const std::string&)>;
using StatusCallback = std::function<void(CameraStatus, const std::string&)>;

/**
 * @brief Abstract camera interface
 * 
 * This interface defines the contract for all camera implementations.
 * It provides hardware abstraction for different camera types while
 * maintaining medical device safety and compliance requirements.
 * 
 * Thread Safety: All methods must be thread-safe unless otherwise noted.
 * Memory Management: Uses RAII and smart pointers for safety.
 * Error Handling: All methods use error codes and exceptions appropriately.
 * 
 * Traceability: REQ-HW-001 (Hardware abstraction)
 */
class ICamera {
public:
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived classes.
     */
    virtual ~ICamera() = default;

    /**
     * @brief Connect to camera hardware
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Establishes connection to the camera hardware and performs
     * initial validation. Must be called before any other operations.
     * 
     * Traceability: REQ-HW-002 (Camera initialization)
     */
    virtual CameraError connect() = 0;

    /**
     * @brief Disconnect from camera hardware
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Safely disconnects from camera hardware and releases resources.
     * Should be called during shutdown or error recovery.
     * 
     * Traceability: REQ-HW-003 (Safe shutdown)
     */
    virtual CameraError disconnect() = 0;

    /**
     * @brief Start frame capture
     * @param config Camera configuration parameters
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Begins continuous frame capture with specified configuration.
     * Camera must be connected before calling this method.
     * 
     * Traceability: REQ-HW-004 (Frame capture)
     */
    virtual CameraError startCapture(const CameraConfig& config) = 0;

    /**
     * @brief Stop frame capture
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Stops frame capture and flushes any pending frames.
     * Resources are maintained for quick restart.
     * 
     * Traceability: REQ-HW-005 (Capture control)
     */
    virtual CameraError stopCapture() = 0;

    /**
     * @brief Get latest depth frame
     * @param frame Output depth frame data
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Retrieves the most recent depth frame from the camera.
     * Frame data includes validation and integrity checks.
     * 
     * Thread Safety: This method is thread-safe.
     * Traceability: REQ-HW-006 (Depth data access)
     */
    virtual CameraError getDepthFrame(DepthFrame& frame) = 0;

    /**
     * @brief Get latest RGB frame
     * @param frame Output RGB frame data
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Retrieves the most recent RGB frame from the camera.
     * Frame data includes quality metrics and validation.
     * 
     * Thread Safety: This method is thread-safe.
     * Traceability: REQ-HW-007 (RGB data access)
     */
    virtual CameraError getRGBFrame(RGBFrame& frame) = 0;

    /**
     * @brief Get synchronized frames
     * @param rgb_frame Output RGB frame data
     * @param depth_frame Output depth frame data
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Retrieves temporally synchronized RGB and depth frames.
     * Critical for accurate pose estimation and analysis.
     * 
     * Thread Safety: This method is thread-safe.
     * Traceability: REQ-HW-008 (Synchronized capture)
     */
    virtual CameraError getSynchronizedFrames(RGBFrame& rgb_frame, DepthFrame& depth_frame) = 0;

    /**
     * @brief Get camera capabilities
     * @return Camera capabilities structure
     * 
     * Returns hardware capabilities and specifications.
     * Used for validation and configuration.
     * 
     * Traceability: REQ-HW-009 (Hardware identification)
     */
    virtual CameraCapabilities getCapabilities() const = 0;

    /**
     * @brief Get current camera status
     * @return Current camera status
     * 
     * Returns the current operational status of the camera.
     * Used for monitoring and diagnostics.
     * 
     * Thread Safety: This method is thread-safe.
     * Traceability: REQ-HW-010 (Status monitoring)
     */
    virtual CameraStatus getStatus() const = 0;

    /**
     * @brief Get last error information
     * @return Last error code and message
     * 
     * Returns detailed information about the last error.
     * Used for debugging and error reporting.
     */
    virtual std::pair<CameraError, std::string> getLastError() const = 0;

    /**
     * @brief Set frame callback
     * @param callback Callback function for new frames
     * 
     * Sets callback function to be called when new synchronized
     * frames are available. Used for real-time processing.
     * 
     * Thread Safety: Callback will be called from internal thread.
     * Traceability: REQ-HW-011 (Real-time processing)
     */
    virtual void setFrameCallback(FrameCallback callback) = 0;

    /**
     * @brief Set error callback
     * @param callback Callback function for errors
     * 
     * Sets callback function to be called when errors occur.
     * Used for error monitoring and recovery.
     */
    virtual void setErrorCallback(ErrorCallback callback) = 0;

    /**
     * @brief Set status callback
     * @param callback Callback function for status changes
     * 
     * Sets callback function to be called when camera status changes.
     * Used for monitoring and diagnostics.
     */
    virtual void setStatusCallback(StatusCallback callback) = 0;

    /**
     * @brief Perform camera calibration
     * @return CameraError::SUCCESS on success, error code otherwise
     * 
     * Performs camera calibration for accurate depth measurements.
     * Required for medical device accuracy compliance.
     * 
     * Traceability: REQ-HW-012 (Calibration)
     */
    virtual CameraError calibrate() = 0;

    /**
     * @brief Validate camera operation
     * @return CameraError::SUCCESS if validation passes, error code otherwise
     * 
     * Performs comprehensive validation of camera operation.
     * Used for medical device safety compliance.
     * 
     * Traceability: REQ-HW-013 (Validation)
     */
    virtual CameraError validate() = 0;

    /**
     * @brief Get performance metrics
     * @return Performance metrics structure
     * 
     * Returns current performance metrics for monitoring.
     * Used for optimization and compliance reporting.
     */
    virtual struct {
        float current_fps;
        float average_latency_ms;
        int dropped_frames;
        float cpu_usage_percent;
        float memory_usage_mb;
        float temperature_celsius;
    } getPerformanceMetrics() const = 0;
};

/**
 * @brief Camera factory interface
 * 
 * Factory pattern for creating camera instances based on hardware type.
 * Enables runtime selection of camera implementation.
 */
class ICameraFactory {
public:
    virtual ~ICameraFactory() = default;
    
    /**
     * @brief Create camera instance
     * @param camera_type Camera type identifier ("d435", "femto_mega")
     * @return Unique pointer to camera instance, nullptr on failure
     */
    virtual std::unique_ptr<ICamera> createCamera(const std::string& camera_type) = 0;
    
    /**
     * @brief Get available camera types
     * @return Vector of available camera type identifiers
     */
    virtual std::vector<std::string> getAvailableCameraTypes() const = 0;
    
    /**
     * @brief Detect connected cameras
     * @return Vector of detected camera identifiers
     */
    virtual std::vector<std::string> detectConnectedCameras() const = 0;
};

} // namespace hardware
} // namespace therapy_device
