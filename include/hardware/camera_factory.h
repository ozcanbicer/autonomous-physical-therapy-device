/**
 * @file camera_factory.h
 * @brief Camera factory for hardware abstraction
 * 
 * Unified factory implementation that provides runtime camera selection
 * and automatic hardware detection. Enables seamless migration between
 * development (D435) and production (Femto Mega) hardware.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-HW-FACTORY-001, REQ-HW-FACTORY-002
 */

#pragma once

#include "icamera.h"
#include "d435_camera.h"
#include "femto_mega_camera.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>

namespace therapy_device {
namespace hardware {

/**
 * @brief Hardware detection result
 * 
 * Contains information about detected camera hardware
 * for validation and selection purposes.
 */
struct HardwareDetectionResult {
    std::string camera_type;             ///< Camera type identifier
    std::string model_name;              ///< Hardware model name
    std::string serial_number;           ///< Hardware serial number
    std::string firmware_version;        ///< Firmware version
    bool is_medical_grade;               ///< Medical grade certification
    bool is_production_ready;            ///< Production readiness status
    float detection_confidence;          ///< Detection confidence (0.0-1.0)
    std::string connection_interface;    ///< Connection interface (USB, etc.)
    std::map<std::string, std::string> additional_info;  ///< Additional hardware info
};

/**
 * @brief Camera factory configuration
 * 
 * Configuration options for camera factory behavior.
 */
struct CameraFactoryConfig {
    bool prefer_production_hardware;     ///< Prefer production over development hardware
    bool enable_automatic_detection;     ///< Enable automatic hardware detection
    bool enable_hardware_validation;     ///< Enable hardware validation
    bool enable_medical_compliance_check; ///< Enable medical device compliance checks
    std::string preferred_camera_type;   ///< Preferred camera type ("auto", "d435", "femto_mega")
    int detection_timeout_ms;            ///< Hardware detection timeout
    bool enable_fallback_mode;           ///< Enable fallback to alternative hardware
    bool enable_simulation_mode;         ///< Enable simulation mode for testing
};

/**
 * @brief Unified camera factory
 * 
 * This factory provides unified camera creation with automatic hardware
 * detection and validation. It handles the complexity of different camera
 * types while providing a simple interface for the application.
 * 
 * Key Features:
 * - Automatic hardware detection
 * - Medical device compliance validation
 * - Production/development hardware preference
 * - Fallback mechanisms for reliability
 * - Simulation mode for testing
 * - Thread-safe operation
 * 
 * Thread Safety: All public methods are thread-safe.
 * Singleton Pattern: Use getInstance() for global access.
 * 
 * Traceability: REQ-HW-FACTORY-001 (Unified factory)
 */
class CameraFactory : public ICameraFactory {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to singleton instance
     * 
     * Thread-safe singleton implementation for global access.
     */
    static CameraFactory& getInstance();

    /**
     * @brief Configure factory behavior
     * @param config Factory configuration
     * 
     * Configures factory behavior including hardware preferences
     * and validation settings.
     */
    void configure(const CameraFactoryConfig& config);

    /**
     * @brief Get current configuration
     * @return Current factory configuration
     */
    CameraFactoryConfig getConfiguration() const;

    // ICameraFactory interface implementation
    std::unique_ptr<ICamera> createCamera(const std::string& camera_type) override;
    std::vector<std::string> getAvailableCameraTypes() const override;
    std::vector<std::string> detectConnectedCameras() const override;

    /**
     * @brief Enhanced factory methods
     */

    /**
     * @brief Create camera with automatic selection
     * @return Unique pointer to best available camera, nullptr on failure
     * 
     * Automatically selects the best available camera based on
     * configuration preferences and hardware detection.
     * 
     * Selection Priority:
     * 1. Production hardware (if prefer_production_hardware is true)
     * 2. Medical grade certified hardware
     * 3. Hardware with best capabilities
     * 4. Fallback to any available hardware
     */
    std::unique_ptr<ICamera> createBestCamera();

    /**
     * @brief Detect all connected hardware
     * @return Vector of detected hardware information
     * 
     * Performs comprehensive hardware detection and returns
     * detailed information about all connected cameras.
     */
    std::vector<HardwareDetectionResult> detectAllHardware() const;

    /**
     * @brief Validate hardware compatibility
     * @param camera_type Camera type to validate
     * @return True if hardware is compatible and ready for use
     * 
     * Validates that the specified camera hardware is compatible
     * with the current system and medical device requirements.
     */
    bool validateHardware(const std::string& camera_type) const;

    /**
     * @brief Get hardware capabilities
     * @param camera_type Camera type
     * @return Camera capabilities, empty if not available
     * 
     * Returns hardware capabilities without creating a camera instance.
     * Useful for capability-based selection and validation.
     */
    CameraCapabilities getHardwareCapabilities(const std::string& camera_type) const;

    /**
     * @brief Check medical device compliance
     * @param camera_type Camera type
     * @return True if hardware meets medical device requirements
     * 
     * Validates that the hardware meets IEC 62304 Class C
     * medical device requirements and certifications.
     */
    bool checkMedicalCompliance(const std::string& camera_type) const;

    /**
     * @brief Enable simulation mode
     * @param enable True to enable simulation, false to disable
     * 
     * Enables simulation mode for testing without physical hardware.
     * Creates mock camera instances that generate synthetic data.
     */
    void enableSimulationMode(bool enable);

    /**
     * @brief Check if simulation mode is active
     * @return True if simulation mode is enabled
     */
    bool isSimulationMode() const;

    /**
     * @brief Get factory status
     * @return Factory status information
     * 
     * Returns comprehensive status information about the factory
     * including detected hardware and configuration status.
     */
    struct FactoryStatus {
        bool is_initialized;
        int detected_cameras_count;
        std::string preferred_camera;
        std::string active_configuration;
        bool medical_compliance_verified;
        bool simulation_mode_active;
        std::vector<std::string> available_types;
        std::vector<std::string> error_messages;
        std::chrono::system_clock::time_point last_detection_time;
    } getFactoryStatus() const;

    /**
     * @brief Refresh hardware detection
     * @return Number of detected cameras
     * 
     * Forces a refresh of hardware detection. Useful when
     * hardware is connected/disconnected during runtime.
     */
    int refreshHardwareDetection();

    /**
     * @brief Register custom camera factory
     * @param camera_type Camera type identifier
     * @param factory Custom factory function
     * 
     * Allows registration of custom camera implementations
     * for extensibility and testing purposes.
     */
    using CameraCreatorFunction = std::function<std::unique_ptr<ICamera>()>;
    void registerCustomCamera(const std::string& camera_type, CameraCreatorFunction factory);

    /**
     * @brief Unregister custom camera factory
     * @param camera_type Camera type identifier
     */
    void unregisterCustomCamera(const std::string& camera_type);

private:
    /**
     * @brief Private constructor for singleton
     */
    CameraFactory();

    /**
     * @brief Destructor
     */
    ~CameraFactory() = default;

    /**
     * @brief Delete copy constructor and assignment operator
     */
    CameraFactory(const CameraFactory&) = delete;
    CameraFactory& operator=(const CameraFactory&) = delete;

    /**
     * @brief Internal state
     */
    mutable std::mutex mutex_;
    CameraFactoryConfig config_;
    bool is_initialized_;
    bool simulation_mode_;

    /**
     * @brief Hardware detection cache
     */
    mutable std::vector<HardwareDetectionResult> detection_cache_;
    mutable std::chrono::system_clock::time_point last_detection_time_;
    mutable bool detection_cache_valid_;
    static const std::chrono::minutes DETECTION_CACHE_TIMEOUT{5};

    /**
     * @brief Factory instances
     */
    std::unique_ptr<D435CameraFactory> d435_factory_;
    std::unique_ptr<FemtoMegaCameraFactory> femto_mega_factory_;

    /**
     * @brief Custom camera factories
     */
    std::map<std::string, CameraCreatorFunction> custom_factories_;

    /**
     * @brief Status tracking
     */
    mutable std::vector<std::string> error_messages_;
    mutable bool medical_compliance_verified_;

    /**
     * @brief Private helper methods
     */

    /**
     * @brief Initialize factory
     */
    void initialize();

    /**
     * @brief Perform hardware detection
     * @return Vector of detected hardware
     */
    std::vector<HardwareDetectionResult> performHardwareDetection() const;

    /**
     * @brief Detect D435 hardware
     * @return Detection result, empty if not found
     */
    std::vector<HardwareDetectionResult> detectD435Hardware() const;

    /**
     * @brief Detect Femto Mega hardware
     * @return Detection result, empty if not found
     */
    std::vector<HardwareDetectionResult> detectFemtoMegaHardware() const;

    /**
     * @brief Select best camera from detection results
     * @param detected_hardware Vector of detected hardware
     * @return Best camera type, empty if none suitable
     */
    std::string selectBestCamera(const std::vector<HardwareDetectionResult>& detected_hardware) const;

    /**
     * @brief Validate detection result
     * @param result Detection result to validate
     * @return True if valid and suitable for use
     */
    bool validateDetectionResult(const HardwareDetectionResult& result) const;

    /**
     * @brief Check medical device certification
     * @param result Detection result to check
     * @return True if medical device certified
     */
    bool checkMedicalCertification(const HardwareDetectionResult& result) const;

    /**
     * @brief Create simulation camera
     * @param camera_type Type of camera to simulate
     * @return Unique pointer to simulation camera
     */
    std::unique_ptr<ICamera> createSimulationCamera(const std::string& camera_type) const;

    /**
     * @brief Update error messages
     * @param message Error message to add
     */
    void addErrorMessage(const std::string& message) const;

    /**
     * @brief Clear error messages
     */
    void clearErrorMessages() const;

    /**
     * @brief Invalidate detection cache
     */
    void invalidateDetectionCache() const;

    /**
     * @brief Check if detection cache is valid
     * @return True if cache is valid and not expired
     */
    bool isDetectionCacheValid() const;
};

/**
 * @brief Convenience functions for common use cases
 */

/**
 * @brief Create camera with default configuration
 * @return Unique pointer to best available camera
 * 
 * Convenience function that uses default factory configuration
 * to create the best available camera.
 */
std::unique_ptr<ICamera> createDefaultCamera();

/**
 * @brief Create camera for development
 * @return Unique pointer to development camera (D435 preferred)
 * 
 * Convenience function optimized for development environment.
 * Prefers D435 camera with relaxed validation.
 */
std::unique_ptr<ICamera> createDevelopmentCamera();

/**
 * @brief Create camera for production
 * @return Unique pointer to production camera (Femto Mega preferred)
 * 
 * Convenience function optimized for production environment.
 * Prefers Femto Mega camera with strict validation.
 */
std::unique_ptr<ICamera> createProductionCamera();

/**
 * @brief Create simulation camera for testing
 * @param camera_type Camera type to simulate ("d435" or "femto_mega")
 * @return Unique pointer to simulation camera
 * 
 * Convenience function for creating simulation cameras for testing
 * without requiring physical hardware.
 */
std::unique_ptr<ICamera> createSimulationCamera(const std::string& camera_type = "d435");

} // namespace hardware
} // namespace therapy_device
