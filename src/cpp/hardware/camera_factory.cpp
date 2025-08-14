/**
 * @file camera_factory.cpp
 * @brief Camera factory implementation
 * 
 * Implementation of the unified camera factory that provides runtime
 * camera selection and automatic hardware detection for medical device
 * compliance and seamless hardware migration.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-HW-FACTORY-001, REQ-HW-FACTORY-002
 */

#include "hardware/camera_factory.h"
#include "hardware/simulation_camera.h"
#include "core/logger.h"
#include "utils/string_utils.h"
#include <algorithm>
#include <chrono>
#include <thread>

namespace therapy_device {
namespace hardware {

// Default factory configuration
static const CameraFactoryConfig DEFAULT_CONFIG = {
    .prefer_production_hardware = true,
    .enable_automatic_detection = true,
    .enable_hardware_validation = true,
    .enable_medical_compliance_check = true,
    .preferred_camera_type = "auto",
    .detection_timeout_ms = 5000,
    .enable_fallback_mode = true,
    .enable_simulation_mode = false
};

CameraFactory& CameraFactory::getInstance() {
    static CameraFactory instance;
    return instance;
}

CameraFactory::CameraFactory() 
    : config_(DEFAULT_CONFIG)
    , is_initialized_(false)
    , simulation_mode_(false)
    , detection_cache_valid_(false)
    , medical_compliance_verified_(false) {
    
    initialize();
}

void CameraFactory::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (is_initialized_) {
        return;
    }
    
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::INFO, "CameraFactory", "Initializing camera factory");
    
    // Initialize sub-factories
    d435_factory_ = std::make_unique<D435CameraFactory>();
    femto_mega_factory_ = std::make_unique<FemtoMegaCameraFactory>();
    
    // Clear error messages
    clearErrorMessages();
    
    // Perform initial hardware detection if enabled
    if (config_.enable_automatic_detection) {
        try {
            auto detected = performHardwareDetection();
            logger.log(core::LogLevel::INFO, "CameraFactory", 
                      "Initial hardware detection found " + std::to_string(detected.size()) + " cameras");
            
            // Verify medical compliance if required
            if (config_.enable_medical_compliance_check) {
                medical_compliance_verified_ = true;
                for (const auto& result : detected) {
                    if (!checkMedicalCertification(result)) {
                        medical_compliance_verified_ = false;
                        addErrorMessage("Hardware " + result.camera_type + " does not meet medical device requirements");
                    }
                }
            }
            
        } catch (const std::exception& e) {
            logger.log(core::LogLevel::WARNING, "CameraFactory", 
                      "Initial hardware detection failed: " + std::string(e.what()));
            addErrorMessage("Initial hardware detection failed: " + std::string(e.what()));
        }
    }
    
    is_initialized_ = true;
    logger.log(core::LogLevel::INFO, "CameraFactory", "Camera factory initialization completed");
}

void CameraFactory::configure(const CameraFactoryConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::INFO, "CameraFactory", "Updating factory configuration");
    
    config_ = config;
    
    // Invalidate detection cache if detection settings changed
    invalidateDetectionCache();
    
    // Update simulation mode
    simulation_mode_ = config_.enable_simulation_mode;
    
    logger.log(core::LogLevel::INFO, "CameraFactory", 
              "Configuration updated - prefer_production: " + 
              std::string(config_.prefer_production_hardware ? "true" : "false"));
}

CameraFactoryConfig CameraFactory::getConfiguration() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_;
}

std::unique_ptr<ICamera> CameraFactory::createCamera(const std::string& camera_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::INFO, "CameraFactory", 
              "Creating camera of type: " + camera_type);
    
    // Handle simulation mode
    if (simulation_mode_) {
        logger.log(core::LogLevel::INFO, "CameraFactory", "Creating simulation camera");
        return createSimulationCamera(camera_type);
    }
    
    // Handle automatic selection
    if (camera_type == "auto") {
        return createBestCamera();
    }
    
    // Check custom factories first
    auto custom_it = custom_factories_.find(camera_type);
    if (custom_it != custom_factories_.end()) {
        logger.log(core::LogLevel::INFO, "CameraFactory", "Using custom factory for " + camera_type);
        try {
            return custom_it->second();
        } catch (const std::exception& e) {
            logger.log(core::LogLevel::ERROR, "CameraFactory", 
                      "Custom factory failed: " + std::string(e.what()));
            addErrorMessage("Custom factory failed: " + std::string(e.what()));
            return nullptr;
        }
    }
    
    // Create specific camera type
    std::unique_ptr<ICamera> camera = nullptr;
    
    try {
        if (camera_type == "d435") {
            camera = d435_factory_->createCamera(camera_type);
        } else if (camera_type == "femto_mega") {
            camera = femto_mega_factory_->createCamera(camera_type);
        } else {
            logger.log(core::LogLevel::ERROR, "CameraFactory", 
                      "Unknown camera type: " + camera_type);
            addErrorMessage("Unknown camera type: " + camera_type);
            return nullptr;
        }
        
        // Validate created camera if required
        if (camera && config_.enable_hardware_validation) {
            auto validation_result = camera->validate();
            if (validation_result != CameraError::SUCCESS) {
                logger.log(core::LogLevel::ERROR, "CameraFactory", 
                          "Camera validation failed for " + camera_type);
                addErrorMessage("Camera validation failed for " + camera_type);
                return nullptr;
            }
        }
        
        if (camera) {
            logger.log(core::LogLevel::INFO, "CameraFactory", 
                      "Successfully created camera: " + camera_type);
        } else {
            logger.log(core::LogLevel::ERROR, "CameraFactory", 
                      "Failed to create camera: " + camera_type);
            addErrorMessage("Failed to create camera: " + camera_type);
        }
        
    } catch (const std::exception& e) {
        logger.log(core::LogLevel::ERROR, "CameraFactory", 
                  "Exception creating camera " + camera_type + ": " + std::string(e.what()));
        addErrorMessage("Exception creating camera: " + std::string(e.what()));
        return nullptr;
    }
    
    return camera;
}

std::unique_ptr<ICamera> CameraFactory::createBestCamera() {
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::INFO, "CameraFactory", "Selecting best available camera");
    
    // Detect available hardware
    auto detected_hardware = performHardwareDetection();
    
    if (detected_hardware.empty()) {
        logger.log(core::LogLevel::WARNING, "CameraFactory", "No cameras detected");
        
        if (config_.enable_fallback_mode && config_.enable_simulation_mode) {
            logger.log(core::LogLevel::INFO, "CameraFactory", "Falling back to simulation mode");
            return createSimulationCamera("d435");
        }
        
        addErrorMessage("No cameras detected and fallback disabled");
        return nullptr;
    }
    
    // Select best camera based on configuration
    std::string best_camera_type = selectBestCamera(detected_hardware);
    
    if (best_camera_type.empty()) {
        logger.log(core::LogLevel::ERROR, "CameraFactory", "No suitable camera found");
        addErrorMessage("No suitable camera found");
        return nullptr;
    }
    
    logger.log(core::LogLevel::INFO, "CameraFactory", "Selected camera: " + best_camera_type);
    return createCamera(best_camera_type);
}

std::vector<std::string> CameraFactory::getAvailableCameraTypes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> types;
    
    // Add standard types
    if (!simulation_mode_) {
        auto d435_types = d435_factory_->getAvailableCameraTypes();
        types.insert(types.end(), d435_types.begin(), d435_types.end());
        
        auto femto_types = femto_mega_factory_->getAvailableCameraTypes();
        types.insert(types.end(), femto_types.begin(), femto_types.end());
    }
    
    // Add custom types
    for (const auto& custom : custom_factories_) {
        types.push_back(custom.first);
    }
    
    // Add simulation types if enabled
    if (simulation_mode_) {
        types.push_back("d435_sim");
        types.push_back("femto_mega_sim");
    }
    
    // Remove duplicates
    std::sort(types.begin(), types.end());
    types.erase(std::unique(types.begin(), types.end()), types.end());
    
    return types;
}

std::vector<std::string> CameraFactory::detectConnectedCameras() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::INFO, "CameraFactory", "Detecting connected cameras");
    
    std::vector<std::string> connected_cameras;
    
    if (simulation_mode_) {
        connected_cameras.push_back("d435_sim");
        connected_cameras.push_back("femto_mega_sim");
        return connected_cameras;
    }
    
    try {
        // Detect D435 cameras
        auto d435_cameras = d435_factory_->detectConnectedCameras();
        connected_cameras.insert(connected_cameras.end(), d435_cameras.begin(), d435_cameras.end());
        
        // Detect Femto Mega cameras
        auto femto_cameras = femto_mega_factory_->detectConnectedCameras();
        connected_cameras.insert(connected_cameras.end(), femto_cameras.begin(), femto_cameras.end());
        
        logger.log(core::LogLevel::INFO, "CameraFactory", 
                  "Detected " + std::to_string(connected_cameras.size()) + " connected cameras");
        
    } catch (const std::exception& e) {
        logger.log(core::LogLevel::ERROR, "CameraFactory", 
                  "Camera detection failed: " + std::string(e.what()));
        addErrorMessage("Camera detection failed: " + std::string(e.what()));
    }
    
    return connected_cameras;
}

std::vector<HardwareDetectionResult> CameraFactory::detectAllHardware() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Use cached results if valid
    if (isDetectionCacheValid()) {
        return detection_cache_;
    }
    
    // Perform fresh detection
    auto results = performHardwareDetection();
    
    // Update cache
    detection_cache_ = results;
    last_detection_time_ = std::chrono::system_clock::now();
    detection_cache_valid_ = true;
    
    return results;
}

std::vector<HardwareDetectionResult> CameraFactory::performHardwareDetection() const {
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::DEBUG, "CameraFactory", "Performing hardware detection");
    
    std::vector<HardwareDetectionResult> results;
    
    if (simulation_mode_) {
        // Create simulation results
        HardwareDetectionResult d435_sim = {
            .camera_type = "d435_sim",
            .model_name = "Intel RealSense D435 (Simulation)",
            .serial_number = "SIM001",
            .firmware_version = "1.0.0-sim",
            .is_medical_grade = true,
            .is_production_ready = false,
            .detection_confidence = 1.0f,
            .connection_interface = "Simulation"
        };
        results.push_back(d435_sim);
        
        HardwareDetectionResult femto_sim = {
            .camera_type = "femto_mega_sim",
            .model_name = "ORBBEC Femto Mega (Simulation)",
            .serial_number = "SIM002",
            .firmware_version = "2.0.0-sim",
            .is_medical_grade = true,
            .is_production_ready = true,
            .detection_confidence = 1.0f,
            .connection_interface = "Simulation"
        };
        results.push_back(femto_sim);
        
        return results;
    }
    
    try {
        // Detect D435 hardware
        auto d435_results = detectD435Hardware();
        results.insert(results.end(), d435_results.begin(), d435_results.end());
        
        // Detect Femto Mega hardware
        auto femto_results = detectFemtoMegaHardware();
        results.insert(results.end(), femto_results.begin(), femto_results.end());
        
        logger.log(core::LogLevel::INFO, "CameraFactory", 
                  "Hardware detection completed - found " + std::to_string(results.size()) + " devices");
        
    } catch (const std::exception& e) {
        logger.log(core::LogLevel::ERROR, "CameraFactory", 
                  "Hardware detection exception: " + std::string(e.what()));
        addErrorMessage("Hardware detection exception: " + std::string(e.what()));
    }
    
    return results;
}

std::vector<HardwareDetectionResult> CameraFactory::detectD435Hardware() const {
    std::vector<HardwareDetectionResult> results;
    
#ifdef THERAPY_DEVICE_HAS_REALSENSE
    try {
        // Use D435 factory for detection
        auto connected_d435 = d435_factory_->detectConnectedCameras();
        
        for (const auto& camera_id : connected_d435) {
            HardwareDetectionResult result = {
                .camera_type = "d435",
                .model_name = "Intel RealSense D435",
                .serial_number = camera_id,
                .firmware_version = "Unknown", // Would need to query actual device
                .is_medical_grade = false,     // D435 is development hardware
                .is_production_ready = false,
                .detection_confidence = 0.9f,
                .connection_interface = "USB 3.0"
            };
            
            result.additional_info["sdk_version"] = "librealsense2";
            result.additional_info["depth_technology"] = "Active IR Stereo";
            result.additional_info["fov"] = "87° x 58° x 95°";
            
            results.push_back(result);
        }
        
    } catch (const std::exception& e) {
        auto& logger = core::Logger::getInstance();
        logger.log(core::LogLevel::WARNING, "CameraFactory", 
                  "D435 detection failed: " + std::string(e.what()));
    }
#endif
    
    return results;
}

std::vector<HardwareDetectionResult> CameraFactory::detectFemtoMegaHardware() const {
    std::vector<HardwareDetectionResult> results;
    
#ifdef THERAPY_DEVICE_HAS_ORBBEC
    try {
        // Use Femto Mega factory for detection
        auto connected_femto = femto_mega_factory_->detectConnectedCameras();
        
        for (const auto& camera_id : connected_femto) {
            HardwareDetectionResult result = {
                .camera_type = "femto_mega",
                .model_name = "ORBBEC Femto Mega",
                .serial_number = camera_id,
                .firmware_version = "Unknown", // Would need to query actual device
                .is_medical_grade = true,      // Femto Mega is medical grade
                .is_production_ready = true,
                .detection_confidence = 0.95f,
                .connection_interface = "USB 3.0"
            };
            
            result.additional_info["sdk_version"] = "OrbbecSDK";
            result.additional_info["depth_technology"] = "Structured Light";
            result.additional_info["accuracy"] = "±1mm @ 1m";
            result.additional_info["medical_certification"] = "Available";
            
            results.push_back(result);
        }
        
    } catch (const std::exception& e) {
        auto& logger = core::Logger::getInstance();
        logger.log(core::LogLevel::WARNING, "CameraFactory", 
                  "Femto Mega detection failed: " + std::string(e.what()));
    }
#endif
    
    return results;
}

std::string CameraFactory::selectBestCamera(const std::vector<HardwareDetectionResult>& detected_hardware) const {
    if (detected_hardware.empty()) {
        return "";
    }
    
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::DEBUG, "CameraFactory", "Selecting best camera from detected hardware");
    
    // Sort cameras by preference
    std::vector<HardwareDetectionResult> sorted_hardware = detected_hardware;
    
    std::sort(sorted_hardware.begin(), sorted_hardware.end(), 
              [this](const HardwareDetectionResult& a, const HardwareDetectionResult& b) {
        
        // Medical grade certification takes priority
        if (config_.enable_medical_compliance_check) {
            if (a.is_medical_grade != b.is_medical_grade) {
                return a.is_medical_grade > b.is_medical_grade;
            }
        }
        
        // Production readiness
        if (config_.prefer_production_hardware) {
            if (a.is_production_ready != b.is_production_ready) {
                return a.is_production_ready > b.is_production_ready;
            }
        }
        
        // Detection confidence
        if (std::abs(a.detection_confidence - b.detection_confidence) > 0.01f) {
            return a.detection_confidence > b.detection_confidence;
        }
        
        // Prefer specific camera type if configured
        if (config_.preferred_camera_type != "auto") {
            if (a.camera_type == config_.preferred_camera_type) return true;
            if (b.camera_type == config_.preferred_camera_type) return false;
        }
        
        // Default preference: Femto Mega > D435
        if (a.camera_type != b.camera_type) {
            return a.camera_type == "femto_mega";
        }
        
        return false;
    });
    
    // Select the best camera that passes validation
    for (const auto& result : sorted_hardware) {
        if (validateDetectionResult(result)) {
            logger.log(core::LogLevel::INFO, "CameraFactory", 
                      "Selected camera: " + result.camera_type + " (" + result.model_name + ")");
            return result.camera_type;
        }
    }
    
    logger.log(core::LogLevel::WARNING, "CameraFactory", "No suitable camera found after validation");
    return "";
}

bool CameraFactory::validateDetectionResult(const HardwareDetectionResult& result) const {
    // Check medical compliance if required
    if (config_.enable_medical_compliance_check && !checkMedicalCertification(result)) {
        return false;
    }
    
    // Check production readiness if required
    if (config_.prefer_production_hardware && !result.is_production_ready) {
        // Allow non-production hardware if no production hardware is available
        // This will be caught in the sorting logic
    }
    
    // Check detection confidence
    if (result.detection_confidence < 0.5f) {
        return false;
    }
    
    return true;
}

bool CameraFactory::checkMedicalCertification(const HardwareDetectionResult& result) const {
    // Check if hardware has medical device certification
    return result.is_medical_grade;
}

std::unique_ptr<ICamera> CameraFactory::createSimulationCamera(const std::string& camera_type) const {
    auto& logger = core::Logger::getInstance();
    logger.log(core::LogLevel::INFO, "CameraFactory", "Creating simulation camera: " + camera_type);
    
    try {
        return std::make_unique<SimulationCamera>(camera_type);
    } catch (const std::exception& e) {
        logger.log(core::LogLevel::ERROR, "CameraFactory", 
                  "Failed to create simulation camera: " + std::string(e.what()));
        addErrorMessage("Failed to create simulation camera: " + std::string(e.what()));
        return nullptr;
    }
}

void CameraFactory::addErrorMessage(const std::string& message) const {
    error_messages_.push_back(message);
    
    // Limit error message history
    if (error_messages_.size() > 100) {
        error_messages_.erase(error_messages_.begin());
    }
}

void CameraFactory::clearErrorMessages() const {
    error_messages_.clear();
}

void CameraFactory::invalidateDetectionCache() const {
    detection_cache_valid_ = false;
    detection_cache_.clear();
}

bool CameraFactory::isDetectionCacheValid() const {
    if (!detection_cache_valid_) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    auto cache_age = now - last_detection_time_;
    
    return cache_age < DETECTION_CACHE_TIMEOUT;
}

// Convenience functions implementation
std::unique_ptr<ICamera> createDefaultCamera() {
    return CameraFactory::getInstance().createBestCamera();
}

std::unique_ptr<ICamera> createDevelopmentCamera() {
    auto& factory = CameraFactory::getInstance();
    
    CameraFactoryConfig dev_config = factory.getConfiguration();
    dev_config.prefer_production_hardware = false;
    dev_config.enable_medical_compliance_check = false;
    dev_config.preferred_camera_type = "d435";
    
    factory.configure(dev_config);
    
    return factory.createBestCamera();
}

std::unique_ptr<ICamera> createProductionCamera() {
    auto& factory = CameraFactory::getInstance();
    
    CameraFactoryConfig prod_config = factory.getConfiguration();
    prod_config.prefer_production_hardware = true;
    prod_config.enable_medical_compliance_check = true;
    prod_config.preferred_camera_type = "femto_mega";
    prod_config.enable_simulation_mode = false;
    
    factory.configure(prod_config);
    
    return factory.createBestCamera();
}

std::unique_ptr<ICamera> createSimulationCamera(const std::string& camera_type) {
    auto& factory = CameraFactory::getInstance();
    factory.enableSimulationMode(true);
    
    return factory.createCamera(camera_type + "_sim");
}

} // namespace hardware
} // namespace therapy_device
