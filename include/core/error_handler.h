/**
 * @file error_handler.h
 * @brief Centralized error handling system for medical device compliance
 * 
 * Comprehensive error handling with recovery mechanisms, audit trails,
 * and IEC 62304 Class C compliance for medical device safety.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-ERR-001, REQ-ERR-002, REQ-ERR-003
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <functional>
#include <chrono>
#include <atomic>
#include <queue>
#include <thread>
#include <condition_variable>

namespace therapy_device {
namespace core {

/**
 * @brief Error severity levels
 * 
 * Hierarchical error severity levels for medical device compliance.
 * Each level defines the appropriate response and escalation requirements.
 */
enum class ErrorSeverity {
    INFORMATIONAL = 0,  ///< Informational messages (not errors)
    LOW = 1,           ///< Low severity - system continues normally
    MEDIUM = 2,        ///< Medium severity - may affect functionality
    HIGH = 3,          ///< High severity - significant impact on operation
    CRITICAL = 4,      ///< Critical - system safety may be compromised
    FATAL = 5          ///< Fatal - immediate shutdown required
};

/**
 * @brief Error categories for classification
 * 
 * Categories help organize errors by type and determine
 * appropriate handling strategies.
 */
enum class ErrorCategory {
    SYSTEM,            ///< System-level errors
    HARDWARE,          ///< Hardware-related errors
    SOFTWARE,          ///< Software logic errors
    COMMUNICATION,     ///< Network/communication errors
    SECURITY,          ///< Security-related errors
    MEDICAL,           ///< Medical device specific errors
    USER,              ///< User input/interaction errors
    CONFIGURATION,     ///< Configuration-related errors
    PERFORMANCE,       ///< Performance-related errors
    COMPLIANCE         ///< Regulatory compliance errors
};

/**
 * @brief Error recovery action types
 * 
 * Defines the type of recovery action that should be taken.
 */
enum class RecoveryAction {
    NONE,              ///< No recovery action needed
    RETRY,             ///< Retry the failed operation
    RESTART_COMPONENT, ///< Restart the affected component
    RESTART_SYSTEM,    ///< Restart the entire system
    SAFE_SHUTDOWN,     ///< Perform safe shutdown
    EMERGENCY_STOP,    ///< Emergency stop all operations
    USER_INTERVENTION, ///< Require user intervention
    MAINTENANCE_MODE,  ///< Switch to maintenance mode
    FALLBACK_MODE,     ///< Switch to fallback/safe mode
    ESCALATE          ///< Escalate to higher authority
};

/**
 * @brief Error context information
 * 
 * Additional context information about the error occurrence.
 */
struct ErrorContext {
    std::string component;              ///< Component that generated the error
    std::string function;               ///< Function where error occurred
    std::string file;                   ///< Source file name
    int line;                          ///< Line number in source file
    std::string thread_id;             ///< Thread identifier
    std::string session_id;            ///< Patient session ID (if applicable)
    std::string user_id;               ///< User ID (if applicable)
    std::string device_id;             ///< Device identifier
    std::map<std::string, std::string> additional_info;  ///< Additional context data
    std::string stack_trace;           ///< Stack trace (if available)
    std::chrono::system_clock::time_point timestamp;     ///< Error occurrence time
};

/**
 * @brief Error information structure
 * 
 * Complete error information with all metadata required
 * for medical device compliance and traceability.
 */
struct ErrorInfo {
    int error_code;                    ///< Unique error code
    std::string error_message;         ///< Human-readable error message
    ErrorSeverity severity;            ///< Error severity level
    ErrorCategory category;            ///< Error category
    RecoveryAction recovery_action;    ///< Recommended recovery action
    ErrorContext context;              ///< Error context information
    std::vector<ErrorInfo> related_errors;  ///< Related/cascading errors
    
    // Medical device specific fields
    bool affects_patient_safety;       ///< Whether error affects patient safety
    bool requires_immediate_attention; ///< Whether error requires immediate attention
    std::string regulatory_classification; ///< Regulatory classification
    std::string mitigation_procedure;  ///< Documented mitigation procedure
    
    // Tracking information
    uint64_t occurrence_count;         ///< Number of times this error occurred
    std::chrono::system_clock::time_point first_occurrence; ///< First occurrence time
    std::chrono::system_clock::time_point last_occurrence;  ///< Last occurrence time
    std::vector<std::chrono::system_clock::time_point> occurrence_history; ///< History of occurrences
    
    // Resolution tracking
    bool is_resolved;                  ///< Whether error has been resolved
    std::string resolution_description; ///< Description of resolution
    std::string resolved_by;           ///< Who resolved the error
    std::chrono::system_clock::time_point resolution_time; ///< When error was resolved
};

/**
 * @brief Error handler interface for custom error handling
 * 
 * Interface for implementing custom error handling strategies.
 */
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    /**
     * @brief Handle error
     * @param error_info Error information
     * @return True if error was handled successfully
     */
    virtual bool handleError(const ErrorInfo& error_info) = 0;
    
    /**
     * @brief Check if handler can handle specific error
     * @param error_code Error code
     * @param category Error category
     * @return True if handler can handle the error
     */
    virtual bool canHandle(int error_code, ErrorCategory category) const = 0;
};

/**
 * @brief Error recovery interface
 * 
 * Interface for implementing error recovery strategies.
 */
class IErrorRecovery {
public:
    virtual ~IErrorRecovery() = default;
    
    /**
     * @brief Attempt error recovery
     * @param error_info Error information
     * @return True if recovery was successful
     */
    virtual bool attemptRecovery(const ErrorInfo& error_info) = 0;
    
    /**
     * @brief Check if recovery is possible for error
     * @param error_code Error code
     * @param category Error category
     * @return True if recovery is possible
     */
    virtual bool canRecover(int error_code, ErrorCategory category) const = 0;
    
    /**
     * @brief Get recovery time estimate
     * @param error_info Error information
     * @return Estimated recovery time in milliseconds
     */
    virtual int getRecoveryTimeEstimate(const ErrorInfo& error_info) const = 0;
};

/**
 * @brief Error notification interface
 * 
 * Interface for error notification implementations.
 */
class IErrorNotifier {
public:
    virtual ~IErrorNotifier() = default;
    
    /**
     * @brief Send error notification
     * @param error_info Error information
     * @return True if notification was sent successfully
     */
    virtual bool notify(const ErrorInfo& error_info) = 0;
    
    /**
     * @brief Check if notifier should handle specific error
     * @param severity Error severity
     * @param category Error category
     * @return True if notifier should handle the error
     */
    virtual bool shouldNotify(ErrorSeverity severity, ErrorCategory category) const = 0;
};

/**
 * @brief Centralized error handler
 * 
 * Medical device compliant error handling system with comprehensive
 * error tracking, recovery mechanisms, and audit trail support.
 * 
 * Key Features:
 * - IEC 62304 Class C compliance
 * - Thread-safe operation
 * - Error categorization and prioritization
 * - Automatic recovery mechanisms
 * - Audit trail and reporting
 * - Real-time error monitoring
 * - Escalation procedures
 * - Medical device safety integration
 * 
 * Thread Safety: All public methods are thread-safe.
 * Performance: Asynchronous error processing for minimal impact.
 * Compliance: Full audit trail and regulatory reporting.
 * 
 * Traceability: REQ-ERR-001 (Centralized error handling)
 */
class ErrorHandler {
public:
    /**
     * @brief Get singleton error handler instance
     * @return Reference to singleton instance
     */
    static ErrorHandler& getInstance();

    /**
     * @brief Error handler configuration
     */
    struct Configuration {
        bool enable_automatic_recovery = true;
        bool enable_error_notifications = true;
        bool enable_audit_trail = true;
        bool enable_real_time_monitoring = true;
        int max_error_history = 10000;
        int max_recovery_attempts = 3;
        int recovery_timeout_ms = 30000;
        std::string audit_file_path = "error_audit.log";
        std::string error_database_path = "errors.db";
        bool enable_error_aggregation = true;
        int aggregation_window_seconds = 60;
        bool enable_predictive_analysis = false;
        std::string notification_endpoint = "";
        std::vector<ErrorSeverity> notification_levels = {
            ErrorSeverity::CRITICAL, ErrorSeverity::FATAL
        };
    };
    
    /**
     * @brief Configure error handler
     * @param config Configuration settings
     */
    void configure(const Configuration& config);
    
    /**
     * @brief Get current configuration
     * @return Current configuration
     */
    Configuration getConfiguration() const;

    /**
     * @brief Core error handling methods
     */
    
    /**
     * @brief Handle error with full context
     * @param error_code Unique error code
     * @param error_message Error message
     * @param severity Error severity
     * @param category Error category
     * @param context Error context
     * @return True if error was handled successfully
     */
    bool handleError(int error_code, const std::string& error_message,
                    ErrorSeverity severity, ErrorCategory category,
                    const ErrorContext& context = {});
    
    /**
     * @brief Handle error with minimal information
     * @param error_code Unique error code
     * @param error_message Error message
     * @param component Component name
     * @return True if error was handled successfully
     */
    bool handleError(int error_code, const std::string& error_message,
                    const std::string& component = "");
    
    /**
     * @brief Handle error from ErrorInfo structure
     * @param error_info Complete error information
     * @return True if error was handled successfully
     */
    bool handleError(const ErrorInfo& error_info);
    
    /**
     * @brief Report error without handling (for logging only)
     * @param error_info Error information
     */
    void reportError(const ErrorInfo& error_info);

    /**
     * @brief Error recovery methods
     */
    
    /**
     * @brief Attempt automatic recovery for error
     * @param error_code Error code
     * @return True if recovery was successful
     */
    bool attemptRecovery(int error_code);
    
    /**
     * @brief Register custom recovery handler
     * @param error_code Error code (or 0 for all errors)
     * @param category Error category
     * @param recovery_handler Recovery handler
     */
    void registerRecoveryHandler(int error_code, ErrorCategory category,
                               std::shared_ptr<IErrorRecovery> recovery_handler);
    
    /**
     * @brief Unregister recovery handler
     * @param error_code Error code
     * @param category Error category
     */
    void unregisterRecoveryHandler(int error_code, ErrorCategory category);

    /**
     * @brief Custom error handlers
     */
    
    /**
     * @brief Register custom error handler
     * @param error_code Error code (or 0 for all errors)
     * @param category Error category
     * @param error_handler Custom error handler
     */
    void registerErrorHandler(int error_code, ErrorCategory category,
                            std::shared_ptr<IErrorHandler> error_handler);
    
    /**
     * @brief Unregister error handler
     * @param error_code Error code
     * @param category Error category
     */
    void unregisterErrorHandler(int error_code, ErrorCategory category);

    /**
     * @brief Error notifications
     */
    
    /**
     * @brief Register error notifier
     * @param notifier Error notifier
     */
    void registerNotifier(std::shared_ptr<IErrorNotifier> notifier);
    
    /**
     * @brief Unregister error notifier
     * @param notifier Error notifier to remove
     */
    void unregisterNotifier(std::shared_ptr<IErrorNotifier> notifier);

    /**
     * @brief Error information and statistics
     */
    
    /**
     * @brief Get error information by code
     * @param error_code Error code
     * @return Error information, nullptr if not found
     */
    std::shared_ptr<ErrorInfo> getErrorInfo(int error_code) const;
    
    /**
     * @brief Get all active errors
     * @return Vector of active error information
     */
    std::vector<ErrorInfo> getActiveErrors() const;
    
    /**
     * @brief Get error history
     * @param start_time Start time for history
     * @param end_time End time for history
     * @param severity_filter Minimum severity level
     * @return Vector of historical errors
     */
    std::vector<ErrorInfo> getErrorHistory(
        const std::chrono::system_clock::time_point& start_time = {},
        const std::chrono::system_clock::time_point& end_time = {},
        ErrorSeverity severity_filter = ErrorSeverity::INFORMATIONAL) const;
    
    /**
     * @brief Get error statistics
     * @return Error statistics
     */
    struct ErrorStatistics {
        int total_errors;
        int active_errors;
        int resolved_errors;
        std::map<ErrorSeverity, int> errors_by_severity;
        std::map<ErrorCategory, int> errors_by_category;
        std::map<int, int> errors_by_code;
        int successful_recoveries;
        int failed_recoveries;
        double average_resolution_time_seconds;
        std::chrono::system_clock::time_point oldest_active_error;
        std::chrono::system_clock::time_point last_error_time;
    } getStatistics() const;

    /**
     * @brief Error resolution
     */
    
    /**
     * @brief Mark error as resolved
     * @param error_code Error code
     * @param resolution_description Description of resolution
     * @param resolved_by Who resolved the error
     * @return True if error was marked as resolved
     */
    bool resolveError(int error_code, const std::string& resolution_description,
                     const std::string& resolved_by);
    
    /**
     * @brief Clear resolved errors from active list
     * @param older_than Clear errors resolved before this time (optional)
     * @return Number of errors cleared
     */
    int clearResolvedErrors(const std::chrono::system_clock::time_point& older_than = {});

    /**
     * @brief Medical device specific methods
     */
    
    /**
     * @brief Check if any active errors affect patient safety
     * @return True if patient safety is affected
     */
    bool isPatientSafetyAffected() const;
    
    /**
     * @brief Get all patient safety related errors
     * @return Vector of patient safety errors
     */
    std::vector<ErrorInfo> getPatientSafetyErrors() const;
    
    /**
     * @brief Generate regulatory compliance report
     * @param start_time Start time for report
     * @param end_time End time for report
     * @return Compliance report content
     */
    std::string generateComplianceReport(
        const std::chrono::system_clock::time_point& start_time,
        const std::chrono::system_clock::time_point& end_time) const;
    
    /**
     * @brief Perform medical device safety check
     * @return True if system is safe for medical use
     */
    bool performSafetyCheck() const;

    /**
     * @brief System control methods
     */
    
    /**
     * @brief Request safe shutdown due to critical errors
     * @param reason Reason for shutdown
     * @return True if shutdown was initiated
     */
    bool requestSafeShutdown(const std::string& reason);
    
    /**
     * @brief Check if safe shutdown is in progress
     * @return True if shutdown is in progress
     */
    bool isShutdownInProgress() const;
    
    /**
     * @brief Enable/disable error handling
     * @param enabled True to enable error handling
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if error handling is enabled
     * @return True if error handling is enabled
     */
    bool isEnabled() const;

private:
    /**
     * @brief Private constructor for singleton
     */
    ErrorHandler();
    
    /**
     * @brief Destructor
     */
    ~ErrorHandler();
    
    /**
     * @brief Delete copy constructor and assignment
     */
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;

    /**
     * @brief Internal state
     */
    mutable std::mutex mutex_;
    Configuration config_;
    std::atomic<bool> enabled_;
    std::atomic<bool> shutdown_in_progress_;
    
    /**
     * @brief Error storage and tracking
     */
    std::map<int, ErrorInfo> active_errors_;
    std::vector<ErrorInfo> error_history_;
    mutable ErrorStatistics statistics_;
    
    /**
     * @brief Custom handlers and notifiers
     */
    std::map<std::pair<int, ErrorCategory>, std::shared_ptr<IErrorHandler>> error_handlers_;
    std::map<std::pair<int, ErrorCategory>, std::shared_ptr<IErrorRecovery>> recovery_handlers_;
    std::vector<std::shared_ptr<IErrorNotifier>> notifiers_;
    
    /**
     * @brief Asynchronous processing
     */
    std::queue<ErrorInfo> processing_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::unique_ptr<std::thread> processing_thread_;
    std::atomic<bool> processing_running_;

    /**
     * @brief Private helper methods
     */
    
    /**
     * @brief Initialize error handler
     */
    void initialize();
    
    /**
     * @brief Shutdown error handler
     */
    void shutdown();
    
    /**
     * @brief Error processing thread function
     */
    void processingThreadFunction();
    
    /**
     * @brief Process single error
     * @param error_info Error to process
     */
    void processError(const ErrorInfo& error_info);
    
    /**
     * @brief Create error context from current state
     * @param component Component name
     * @return Error context
     */
    ErrorContext createErrorContext(const std::string& component) const;
    
    /**
     * @brief Determine recovery action for error
     * @param error_info Error information
     * @return Recommended recovery action
     */
    RecoveryAction determineRecoveryAction(const ErrorInfo& error_info) const;
    
    /**
     * @brief Execute recovery action
     * @param error_info Error information
     * @return True if recovery was successful
     */
    bool executeRecoveryAction(const ErrorInfo& error_info);
    
    /**
     * @brief Send notifications for error
     * @param error_info Error information
     */
    void sendNotifications(const ErrorInfo& error_info);
    
    /**
     * @brief Update error statistics
     * @param error_info Error information
     */
    void updateStatistics(const ErrorInfo& error_info);
    
    /**
     * @brief Write to audit trail
     * @param error_info Error information
     * @param action Action taken
     */
    void writeAuditTrail(const ErrorInfo& error_info, const std::string& action);
    
    /**
     * @brief Get current thread ID as string
     * @return Thread ID string
     */
    std::string getCurrentThreadId() const;
    
    /**
     * @brief Get stack trace if available
     * @return Stack trace string
     */
    std::string getStackTrace() const;
};

/**
 * @brief Convenience macros for error handling
 */
#define THERAPY_HANDLE_ERROR(code, message) \
    therapy_device::core::ErrorHandler::getInstance().handleError(code, message, __FUNCTION__)

#define THERAPY_HANDLE_ERROR_CTX(code, message, severity, category, context) \
    therapy_device::core::ErrorHandler::getInstance().handleError(code, message, severity, category, context)

#define THERAPY_REPORT_ERROR(error_info) \
    therapy_device::core::ErrorHandler::getInstance().reportError(error_info)

/**
 * @brief Standard error recovery implementations
 */

/**
 * @brief Retry recovery handler
 */
class RetryRecovery : public IErrorRecovery {
public:
    RetryRecovery(int max_attempts = 3, int delay_ms = 1000);
    
    bool attemptRecovery(const ErrorInfo& error_info) override;
    bool canRecover(int error_code, ErrorCategory category) const override;
    int getRecoveryTimeEstimate(const ErrorInfo& error_info) const override;

private:
    int max_attempts_;
    int delay_ms_;
};

/**
 * @brief Component restart recovery handler
 */
class ComponentRestartRecovery : public IErrorRecovery {
public:
    ComponentRestartRecovery(const std::string& component_name);
    
    bool attemptRecovery(const ErrorInfo& error_info) override;
    bool canRecover(int error_code, ErrorCategory category) const override;
    int getRecoveryTimeEstimate(const ErrorInfo& error_info) const override;

private:
    std::string component_name_;
};

/**
 * @brief Email notification handler
 */
class EmailNotifier : public IErrorNotifier {
public:
    EmailNotifier(const std::string& smtp_server, const std::vector<std::string>& recipients);
    
    bool notify(const ErrorInfo& error_info) override;
    bool shouldNotify(ErrorSeverity severity, ErrorCategory category) const override;

private:
    std::string smtp_server_;
    std::vector<std::string> recipients_;
};

} // namespace core
} // namespace therapy_device
