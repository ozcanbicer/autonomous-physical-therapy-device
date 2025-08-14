/**
 * @file application.h
 * @brief Main application class for medical device lifecycle management
 * 
 * Comprehensive application lifecycle management with medical device
 * compliance, safety monitoring, and IEC 62304 Class C requirements.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-APP-001, REQ-APP-002, REQ-APP-003
 */

#pragma once

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <string>
#include <map>
#include <vector>

#include "config_manager.h"
#include "logger.h"
#include "error_handler.h"
#include "hardware/icamera.h"
#include "utils/command_line_parser.h"

namespace therapy_device {
namespace core {

/**
 * @brief Application state enumeration
 * 
 * Represents the current state of the application lifecycle
 * for medical device compliance and safety monitoring.
 */
enum class ApplicationState {
    UNINITIALIZED,     ///< Application not yet initialized
    INITIALIZING,      ///< Initialization in progress
    READY,             ///< Ready to start operations
    STARTING,          ///< Startup in progress
    RUNNING,           ///< Normal operation
    PAUSED,            ///< Temporarily paused
    STOPPING,          ///< Shutdown in progress
    STOPPED,           ///< Stopped normally
    ERROR,             ///< Error state
    MAINTENANCE,       ///< Maintenance mode
    SAFE_MODE,         ///< Safe mode operation
    EMERGENCY_STOP     ///< Emergency stop state
};

/**
 * @brief Application component interface
 * 
 * Interface for application components that need lifecycle management.
 */
class IApplicationComponent {
public:
    virtual ~IApplicationComponent() = default;
    
    /**
     * @brief Initialize component
     * @return True if initialization successful
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Start component operation
     * @return True if start successful
     */
    virtual bool start() = 0;
    
    /**
     * @brief Stop component operation
     * @return True if stop successful
     */
    virtual bool stop() = 0;
    
    /**
     * @brief Shutdown component
     * @return True if shutdown successful
     */
    virtual bool shutdown() = 0;
    
    /**
     * @brief Get component name
     * @return Component name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get component status
     * @return Component status information
     */
    virtual std::string getStatus() const = 0;
    
    /**
     * @brief Perform component health check
     * @return True if component is healthy
     */
    virtual bool performHealthCheck() = 0;
};

/**
 * @brief Application event listener interface
 * 
 * Interface for receiving application lifecycle events.
 */
class IApplicationEventListener {
public:
    virtual ~IApplicationEventListener() = default;
    
    /**
     * @brief Handle application state change
     * @param old_state Previous application state
     * @param new_state New application state
     */
    virtual void onStateChanged(ApplicationState old_state, ApplicationState new_state) = 0;
    
    /**
     * @brief Handle application error
     * @param error_code Error code
     * @param error_message Error message
     */
    virtual void onError(int error_code, const std::string& error_message) = 0;
    
    /**
     * @brief Handle shutdown request
     * @param reason Shutdown reason
     */
    virtual void onShutdownRequested(const std::string& reason) = 0;
};

/**
 * @brief Main application class
 * 
 * Central application lifecycle manager for the Autonomous Physical Therapy Device.
 * Coordinates all system components, manages medical device compliance,
 * and ensures safe operation according to IEC 62304 Class C requirements.
 * 
 * Key Features:
 * - IEC 62304 Class C compliance
 * - Medical device safety monitoring
 * - Component lifecycle management
 * - Real-time health monitoring
 * - Emergency shutdown procedures
 * - Audit trail and logging
 * - Performance monitoring
 * - User session management
 * 
 * Thread Safety: All public methods are thread-safe.
 * Safety: Comprehensive safety checks and emergency procedures.
 * Compliance: Full audit trail and regulatory compliance.
 * 
 * Traceability: REQ-APP-001 (Application lifecycle management)
 */
class Application {
public:
    /**
     * @brief Constructor
     */
    Application();
    
    /**
     * @brief Destructor
     */
    virtual ~Application();

    /**
     * @brief Application configuration
     */
    struct Configuration {
        std::string device_id = "THERAPY_DEVICE_001";
        std::string device_name = "Autonomous Physical Therapy Device";
        std::string firmware_version = "1.0.0";
        bool medical_mode_enabled = true;
        bool safety_monitoring_enabled = true;
        bool performance_monitoring_enabled = true;
        int health_check_interval_seconds = 30;
        int session_timeout_minutes = 30;
        int max_concurrent_sessions = 1;
        std::string preferred_camera = "auto";
        bool enable_ai_processing = true;
        bool enable_communication = true;
        std::string log_level = "INFO";
        std::string config_file = "config/device_config.json";
        bool enable_audit_trail = true;
        bool enable_emergency_procedures = true;
        double safety_timeout_seconds = 5.0;
    };

    /**
     * @brief Lifecycle management
     */
    
    /**
     * @brief Initialize application
     * @param cmd_parser Command line parser with arguments
     * @return True if initialization successful
     */
    bool initialize(const utils::CommandLineParser& cmd_parser);
    
    /**
     * @brief Start application
     * @return True if start successful
     */
    bool start();
    
    /**
     * @brief Run application main loop
     * @return Exit code (0 for success)
     */
    int run();
    
    /**
     * @brief Stop application
     * @return True if stop successful
     */
    bool stop();
    
    /**
     * @brief Shutdown application
     * @return True if shutdown successful
     */
    bool shutdown();
    
    /**
     * @brief Request shutdown
     * @param reason Reason for shutdown
     */
    void requestShutdown(const std::string& reason = "User request");
    
    /**
     * @brief Request emergency stop
     * @param reason Reason for emergency stop
     */
    void requestEmergencyStop(const std::string& reason);

    /**
     * @brief State management
     */
    
    /**
     * @brief Get current application state
     * @return Current application state
     */
    ApplicationState getState() const;
    
    /**
     * @brief Check if application is running
     * @return True if application is in running state
     */
    bool isRunning() const;
    
    /**
     * @brief Check if shutdown is requested
     * @return True if shutdown has been requested
     */
    bool isShutdownRequested() const;
    
    /**
     * @brief Get application uptime
     * @return Uptime duration
     */
    std::chrono::duration<double> getUptime() const;

    /**
     * @brief Component management
     */
    
    /**
     * @brief Register application component
     * @param component Component to register
     */
    void registerComponent(std::shared_ptr<IApplicationComponent> component);
    
    /**
     * @brief Unregister application component
     * @param component_name Name of component to unregister
     */
    void unregisterComponent(const std::string& component_name);
    
    /**
     * @brief Get component by name
     * @param component_name Component name
     * @return Component pointer, nullptr if not found
     */
    std::shared_ptr<IApplicationComponent> getComponent(const std::string& component_name) const;
    
    /**
     * @brief Get all registered components
     * @return Vector of all components
     */
    std::vector<std::shared_ptr<IApplicationComponent>> getAllComponents() const;

    /**
     * @brief Event handling
     */
    
    /**
     * @brief Register event listener
     * @param listener Event listener
     */
    void registerEventListener(std::shared_ptr<IApplicationEventListener> listener);
    
    /**
     * @brief Unregister event listener
     * @param listener Event listener to remove
     */
    void unregisterEventListener(std::shared_ptr<IApplicationEventListener> listener);

    /**
     * @brief Configuration management
     */
    
    /**
     * @brief Set application configuration
     * @param config Application configuration
     */
    void setConfiguration(const Configuration& config);
    
    /**
     * @brief Get current configuration
     * @return Current application configuration
     */
    Configuration getConfiguration() const;
    
    /**
     * @brief Reload configuration from file
     * @return True if reload successful
     */
    bool reloadConfiguration();

    /**
     * @brief Health monitoring
     */
    
    /**
     * @brief Perform system health check
     * @return True if all systems are healthy
     */
    bool performHealthCheck();
    
    /**
     * @brief Get system health status
     * @return Health status information
     */
    struct HealthStatus {
        bool overall_healthy;
        std::map<std::string, bool> component_health;
        std::vector<std::string> health_issues;
        std::chrono::system_clock::time_point last_check_time;
        double cpu_usage_percent;
        double memory_usage_percent;
        double temperature_celsius;
        bool patient_safety_ok;
        bool hardware_ok;
        bool communication_ok;
    } getHealthStatus() const;
    
    /**
     * @brief Enable/disable health monitoring
     * @param enabled True to enable health monitoring
     */
    void setHealthMonitoringEnabled(bool enabled);

    /**
     * @brief Performance monitoring
     */
    
    /**
     * @brief Get performance metrics
     * @return Performance metrics
     */
    struct PerformanceMetrics {
        double cpu_usage_percent;
        double memory_usage_mb;
        double gpu_usage_percent;
        double gpu_memory_usage_mb;
        double disk_usage_percent;
        double network_throughput_mbps;
        int active_threads;
        double frame_rate_fps;
        double processing_latency_ms;
        std::chrono::duration<double> uptime;
        int processed_frames;
        int dropped_frames;
        std::map<std::string, double> component_metrics;
    } getPerformanceMetrics() const;
    
    /**
     * @brief Reset performance counters
     */
    void resetPerformanceCounters();

    /**
     * @brief Medical device specific methods
     */
    
    /**
     * @brief Check if medical mode is enabled
     * @return True if medical mode is enabled
     */
    bool isMedicalModeEnabled() const;
    
    /**
     * @brief Enable/disable medical mode
     * @param enabled True to enable medical mode
     * @param authorization_code Medical authorization code
     * @return True if mode change successful
     */
    bool setMedicalModeEnabled(bool enabled, const std::string& authorization_code = "");
    
    /**
     * @brief Perform medical device safety check
     * @return True if device is safe for medical use
     */
    bool performMedicalSafetyCheck();
    
    /**
     * @brief Get medical device status
     * @return Medical device status information
     */
    struct MedicalDeviceStatus {
        bool medical_mode_active;
        bool patient_safety_verified;
        bool hardware_certified;
        bool software_validated;
        bool calibration_current;
        std::string last_safety_check;
        std::string regulatory_status;
        std::vector<std::string> active_alarms;
        std::vector<std::string> maintenance_required;
    } getMedicalDeviceStatus() const;

    /**
     * @brief Session management
     */
    
    /**
     * @brief Start patient session
     * @param session_id Session identifier
     * @param patient_id Patient identifier (anonymized)
     * @param user_id User identifier
     * @return True if session started successfully
     */
    bool startSession(const std::string& session_id, 
                     const std::string& patient_id = "",
                     const std::string& user_id = "");
    
    /**
     * @brief End patient session
     * @param session_id Session identifier
     * @param reason Reason for ending session
     * @return True if session ended successfully
     */
    bool endSession(const std::string& session_id, const std::string& reason = "Normal completion");
    
    /**
     * @brief Get active session information
     * @return Active session information
     */
    struct SessionInfo {
        std::string session_id;
        std::string patient_id;
        std::string user_id;
        std::chrono::system_clock::time_point start_time;
        std::chrono::duration<double> duration;
        bool is_active;
        std::string status;
        std::map<std::string, std::string> session_data;
    } getActiveSession() const;
    
    /**
     * @brief Check if session is active
     * @return True if a session is currently active
     */
    bool hasActiveSession() const;

    /**
     * @brief Diagnostic and reporting
     */
    
    /**
     * @brief Generate system diagnostic report
     * @return Diagnostic report content
     */
    std::string generateDiagnosticReport() const;
    
    /**
     * @brief Generate compliance report
     * @param start_time Start time for report
     * @param end_time End time for report
     * @return Compliance report content
     */
    std::string generateComplianceReport(
        const std::chrono::system_clock::time_point& start_time,
        const std::chrono::system_clock::time_point& end_time) const;
    
    /**
     * @brief Get application statistics
     * @return Application statistics
     */
    struct ApplicationStatistics {
        std::chrono::system_clock::time_point start_time;
        std::chrono::duration<double> total_uptime;
        int total_sessions;
        int successful_sessions;
        int failed_sessions;
        int system_restarts;
        int emergency_stops;
        int health_check_failures;
        int configuration_changes;
        std::map<ApplicationState, std::chrono::duration<double>> time_in_states;
        std::map<std::string, int> component_failures;
    } getStatistics() const;

private:
    /**
     * @brief Internal state
     */
    mutable std::mutex mutex_;
    std::atomic<ApplicationState> state_;
    std::atomic<bool> shutdown_requested_;
    std::atomic<bool> emergency_stop_requested_;
    Configuration config_;
    
    /**
     * @brief Core system components
     */
    ConfigManager* config_manager_;
    Logger* logger_;
    ErrorHandler* error_handler_;
    
    /**
     * @brief Hardware components
     */
    std::unique_ptr<hardware::ICamera> camera_;
    
    /**
     * @brief Application components
     */
    std::map<std::string, std::shared_ptr<IApplicationComponent>> components_;
    std::vector<std::weak_ptr<IApplicationEventListener>> event_listeners_;
    
    /**
     * @brief Threading and monitoring
     */
    std::unique_ptr<std::thread> main_thread_;
    std::unique_ptr<std::thread> health_monitor_thread_;
    std::unique_ptr<std::thread> performance_monitor_thread_;
    std::atomic<bool> monitoring_enabled_;
    std::condition_variable main_cv_;
    std::mutex main_mutex_;
    
    /**
     * @brief Session management
     */
    SessionInfo current_session_;
    std::atomic<bool> session_active_;
    
    /**
     * @brief Performance and health data
     */
    mutable PerformanceMetrics performance_metrics_;
    mutable HealthStatus health_status_;
    mutable ApplicationStatistics statistics_;
    
    /**
     * @brief Timing information
     */
    std::chrono::system_clock::time_point start_time_;
    std::chrono::system_clock::time_point last_health_check_;
    std::chrono::system_clock::time_point last_performance_update_;

    /**
     * @brief Private helper methods
     */
    
    /**
     * @brief Initialize core systems
     * @return True if initialization successful
     */
    bool initializeCoreystems();
    
    /**
     * @brief Initialize hardware components
     * @return True if initialization successful
     */
    bool initializeHardware();
    
    /**
     * @brief Initialize AI processing
     * @return True if initialization successful
     */
    bool initializeAIProcessing();
    
    /**
     * @brief Initialize communication systems
     * @return True if initialization successful
     */
    bool initializeCommunication();
    
    /**
     * @brief Main application thread function
     */
    void mainThreadFunction();
    
    /**
     * @brief Health monitoring thread function
     */
    void healthMonitorThreadFunction();
    
    /**
     * @brief Performance monitoring thread function
     */
    void performanceMonitorThreadFunction();
    
    /**
     * @brief Change application state
     * @param new_state New application state
     * @param reason Reason for state change
     */
    void changeState(ApplicationState new_state, const std::string& reason = "");
    
    /**
     * @brief Notify event listeners
     * @param old_state Previous state
     * @param new_state New state
     */
    void notifyStateChange(ApplicationState old_state, ApplicationState new_state);
    
    /**
     * @brief Handle error condition
     * @param error_code Error code
     * @param error_message Error message
     */
    void handleError(int error_code, const std::string& error_message);
    
    /**
     * @brief Perform emergency shutdown
     * @param reason Reason for emergency shutdown
     */
    void performEmergencyShutdown(const std::string& reason);
    
    /**
     * @brief Update performance metrics
     */
    void updatePerformanceMetrics();
    
    /**
     * @brief Update health status
     */
    void updateHealthStatus();
    
    /**
     * @brief Update application statistics
     */
    void updateStatistics();
    
    /**
     * @brief Validate medical device compliance
     * @return True if compliant
     */
    bool validateMedicalCompliance() const;
    
    /**
     * @brief Get system resource usage
     */
    void getSystemResourceUsage(double& cpu_percent, double& memory_mb, double& temp_celsius) const;
    
    /**
     * @brief Check component health
     * @param component_name Component to check
     * @return True if component is healthy
     */
    bool checkComponentHealth(const std::string& component_name) const;
};

} // namespace core
} // namespace therapy_device
