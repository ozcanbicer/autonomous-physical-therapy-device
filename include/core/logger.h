/**
 * @file logger.h
 * @brief Centralized logging system for medical device compliance
 * 
 * Thread-safe logging system with medical device traceability,
 * audit trails, and IEC 62304 Class C compliance features.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-LOG-001, REQ-LOG-002, REQ-LOG-003
 */

#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <map>

namespace therapy_device {
namespace core {

/**
 * @brief Log severity levels
 * 
 * Hierarchical log levels for medical device compliance.
 * Each level includes all higher severity levels.
 */
enum class LogLevel {
    TRACE = 0,      ///< Detailed trace information for debugging
    DEBUG = 1,      ///< Debug information for development
    INFO = 2,       ///< General information messages
    WARNING = 3,    ///< Warning conditions that should be noted
    ERROR = 4,      ///< Error conditions that affect functionality
    CRITICAL = 5,   ///< Critical errors that may cause system failure
    FATAL = 6       ///< Fatal errors that require immediate shutdown
};

/**
 * @brief Log entry structure
 * 
 * Complete log entry with medical device traceability information.
 */
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string component;          ///< Component/module that generated the log
    std::string message;            ///< Log message content
    std::string thread_id;          ///< Thread identifier
    uint64_t sequence_number;       ///< Sequential entry number
    
    // Medical device specific fields
    std::string session_id;         ///< Patient session identifier (if applicable)
    std::string device_id;          ///< Device identifier
    std::string user_id;            ///< User identifier (if applicable)
    std::string trace_id;           ///< Distributed tracing identifier
    
    // Additional context
    std::map<std::string, std::string> context;  ///< Additional context data
    
    // Error information (for ERROR and above)
    int error_code;                 ///< Error code (0 for non-error entries)
    std::string stack_trace;        ///< Stack trace (if available)
};

/**
 * @brief Log output destination
 * 
 * Defines where log entries should be written.
 */
enum class LogDestination {
    CONSOLE,        ///< Console/stdout output
    FILE,           ///< File output
    SYSLOG,         ///< System log
    NETWORK,        ///< Network logging service
    AUDIT_TRAIL     ///< Medical device audit trail
};

/**
 * @brief Log formatter interface
 * 
 * Interface for custom log formatting implementations.
 */
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    
    /**
     * @brief Format log entry to string
     * @param entry Log entry to format
     * @return Formatted log string
     */
    virtual std::string format(const LogEntry& entry) const = 0;
};

/**
 * @brief Log output interface
 * 
 * Interface for custom log output implementations.
 */
class ILogOutput {
public:
    virtual ~ILogOutput() = default;
    
    /**
     * @brief Write formatted log entry
     * @param formatted_entry Formatted log string
     * @return True if write successful, false otherwise
     */
    virtual bool write(const std::string& formatted_entry) = 0;
    
    /**
     * @brief Flush any buffered output
     */
    virtual void flush() = 0;
};

/**
 * @brief Medical device compliant logger
 * 
 * Thread-safe, high-performance logging system with medical device
 * compliance features including audit trails, traceability, and
 * regulatory reporting capabilities.
 * 
 * Key Features:
 * - IEC 62304 Class C compliance
 * - Thread-safe operation
 * - Asynchronous logging for performance
 * - Multiple output destinations
 * - Structured logging with context
 * - Audit trail generation
 * - Log rotation and archiving
 * - HIPAA compliant (no PHI in logs)
 * 
 * Thread Safety: All public methods are thread-safe.
 * Performance: Asynchronous logging minimizes impact on critical paths.
 * Compliance: Full audit trail and regulatory reporting support.
 * 
 * Traceability: REQ-LOG-001 (Centralized logging)
 */
class Logger {
public:
    /**
     * @brief Get singleton logger instance
     * @return Reference to singleton logger
     */
    static Logger& getInstance();

    /**
     * @brief Configure logger
     * @param config Logger configuration
     */
    struct Config {
        LogLevel min_level = LogLevel::INFO;
        bool enable_console_output = true;
        bool enable_file_output = true;
        std::string log_file_path = "therapy_device.log";
        size_t max_file_size_mb = 100;
        int max_file_count = 10;
        bool enable_rotation = true;
        bool enable_compression = true;
        bool enable_audit_trail = true;
        std::string audit_file_path = "therapy_device_audit.log";
        bool enable_async_logging = true;
        size_t async_buffer_size = 10000;
        int flush_interval_ms = 1000;
        bool enable_structured_logging = true;
        bool enable_stack_trace = false;  // Only for ERROR and above
        std::string device_id = "THERAPY_DEVICE_001";
    };
    
    void configure(const Config& config);
    
    /**
     * @brief Get current configuration
     * @return Current logger configuration
     */
    Config getConfiguration() const;

    /**
     * @brief Core logging methods
     */
    
    /**
     * @brief Log message with specified level
     * @param level Log level
     * @param component Component name
     * @param message Log message
     * @param context Additional context (optional)
     */
    void log(LogLevel level, const std::string& component, const std::string& message,
             const std::map<std::string, std::string>& context = {});
    
    /**
     * @brief Log with error code
     * @param level Log level
     * @param component Component name
     * @param message Log message
     * @param error_code Error code
     * @param context Additional context (optional)
     */
    void logWithError(LogLevel level, const std::string& component, 
                     const std::string& message, int error_code,
                     const std::map<std::string, std::string>& context = {});

    /**
     * @brief Convenience logging methods
     */
    void trace(const std::string& component, const std::string& message,
               const std::map<std::string, std::string>& context = {});
    
    void debug(const std::string& component, const std::string& message,
               const std::map<std::string, std::string>& context = {});
    
    void info(const std::string& component, const std::string& message,
              const std::map<std::string, std::string>& context = {});
    
    void warning(const std::string& component, const std::string& message,
                 const std::map<std::string, std::string>& context = {});
    
    void error(const std::string& component, const std::string& message,
               const std::map<std::string, std::string>& context = {});
    
    void critical(const std::string& component, const std::string& message,
                  const std::map<std::string, std::string>& context = {});
    
    void fatal(const std::string& component, const std::string& message,
               const std::map<std::string, std::string>& context = {});

    /**
     * @brief Medical device specific logging
     */
    
    /**
     * @brief Log patient session event
     * @param session_id Patient session identifier
     * @param event_type Type of event
     * @param message Event description
     * @param context Additional context
     */
    void logSessionEvent(const std::string& session_id, const std::string& event_type,
                        const std::string& message,
                        const std::map<std::string, std::string>& context = {});
    
    /**
     * @brief Log security event for audit trail
     * @param event_type Security event type
     * @param user_id User identifier (if applicable)
     * @param message Event description
     * @param context Additional context
     */
    void logSecurityEvent(const std::string& event_type, const std::string& user_id,
                         const std::string& message,
                         const std::map<std::string, std::string>& context = {});
    
    /**
     * @brief Log system performance metrics
     * @param component Component name
     * @param metrics Performance metrics
     */
    void logPerformanceMetrics(const std::string& component,
                              const std::map<std::string, double>& metrics);
    
    /**
     * @brief Log compliance event
     * @param compliance_type Type of compliance event
     * @param status Compliance status
     * @param details Event details
     */
    void logComplianceEvent(const std::string& compliance_type, 
                           const std::string& status,
                           const std::string& details);

    /**
     * @brief Configuration methods
     */
    
    /**
     * @brief Set minimum log level
     * @param level Minimum level to log
     */
    void setLogLevel(LogLevel level);
    
    /**
     * @brief Get current log level
     * @return Current minimum log level
     */
    LogLevel getLogLevel() const;
    
    /**
     * @brief Enable/disable console output
     * @param enable True to enable console output
     */
    void enableConsoleOutput(bool enable);
    
    /**
     * @brief Enable/disable file output
     * @param enable True to enable file output
     * @param filename Log file path (optional)
     */
    void enableFileOutput(bool enable, const std::string& filename = "");
    
    /**
     * @brief Add custom output destination
     * @param name Output name
     * @param output Custom output implementation
     */
    void addOutput(const std::string& name, std::unique_ptr<ILogOutput> output);
    
    /**
     * @brief Remove custom output destination
     * @param name Output name to remove
     */
    void removeOutput(const std::string& name);
    
    /**
     * @brief Set custom formatter
     * @param formatter Custom formatter implementation
     */
    void setFormatter(std::unique_ptr<ILogFormatter> formatter);

    /**
     * @brief Control methods
     */
    
    /**
     * @brief Flush all pending log entries
     */
    void flush();
    
    /**
     * @brief Rotate log files
     * @return True if rotation successful
     */
    bool rotateLogFiles();
    
    /**
     * @brief Get logging statistics
     * @return Statistics about logging activity
     */
    struct Statistics {
        uint64_t total_entries;
        uint64_t entries_by_level[7];  // One for each LogLevel
        uint64_t dropped_entries;
        uint64_t flush_count;
        uint64_t rotation_count;
        std::chrono::system_clock::time_point last_entry_time;
        std::chrono::system_clock::time_point last_flush_time;
        std::chrono::system_clock::time_point last_rotation_time;
        size_t current_file_size;
        bool async_queue_full;
    } getStatistics() const;
    
    /**
     * @brief Generate audit report
     * @param start_time Start time for report
     * @param end_time End time for report
     * @return Audit report content
     */
    std::string generateAuditReport(
        const std::chrono::system_clock::time_point& start_time,
        const std::chrono::system_clock::time_point& end_time) const;

private:
    /**
     * @brief Private constructor for singleton
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Delete copy constructor and assignment
     */
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Internal state
     */
    mutable std::mutex config_mutex_;
    Config config_;
    std::atomic<LogLevel> min_level_;
    std::atomic<bool> shutdown_requested_;
    
    /**
     * @brief Sequence number for log entries
     */
    std::atomic<uint64_t> sequence_counter_;
    
    /**
     * @brief Asynchronous logging
     */
    std::queue<LogEntry> log_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::unique_ptr<std::thread> logging_thread_;
    std::atomic<bool> logging_thread_running_;
    
    /**
     * @brief Output destinations
     */
    std::map<std::string, std::unique_ptr<ILogOutput>> outputs_;
    std::mutex outputs_mutex_;
    
    /**
     * @brief Formatter
     */
    std::unique_ptr<ILogFormatter> formatter_;
    std::mutex formatter_mutex_;
    
    /**
     * @brief File management
     */
    std::unique_ptr<std::ofstream> log_file_;
    std::unique_ptr<std::ofstream> audit_file_;
    std::mutex file_mutex_;
    size_t current_file_size_;
    
    /**
     * @brief Statistics
     */
    mutable Statistics stats_;
    mutable std::mutex stats_mutex_;

    /**
     * @brief Private helper methods
     */
    
    /**
     * @brief Initialize logger
     */
    void initialize();
    
    /**
     * @brief Shutdown logger
     */
    void shutdown();
    
    /**
     * @brief Asynchronous logging thread function
     */
    void loggingThreadFunction();
    
    /**
     * @brief Process single log entry
     * @param entry Log entry to process
     */
    void processLogEntry(const LogEntry& entry);
    
    /**
     * @brief Create log entry
     * @param level Log level
     * @param component Component name
     * @param message Log message
     * @param context Additional context
     * @param error_code Error code (0 for non-errors)
     * @return Created log entry
     */
    LogEntry createLogEntry(LogLevel level, const std::string& component,
                           const std::string& message,
                           const std::map<std::string, std::string>& context,
                           int error_code = 0);
    
    /**
     * @brief Check if log level should be processed
     * @param level Log level to check
     * @return True if level should be logged
     */
    bool shouldLog(LogLevel level) const;
    
    /**
     * @brief Initialize file outputs
     */
    void initializeFileOutputs();
    
    /**
     * @brief Rotate log file if needed
     */
    void checkAndRotateLogFile();
    
    /**
     * @brief Update statistics
     * @param entry Log entry
     */
    void updateStatistics(const LogEntry& entry);
    
    /**
     * @brief Get current thread ID as string
     * @return Thread ID string
     */
    std::string getCurrentThreadId() const;
    
    /**
     * @brief Get stack trace (if enabled)
     * @return Stack trace string
     */
    std::string getStackTrace() const;
    
    /**
     * @brief Generate trace ID
     * @return Unique trace ID
     */
    std::string generateTraceId() const;
};

/**
 * @brief Standard log formatter
 * 
 * Default formatter that produces human-readable log output
 * with medical device compliance information.
 */
class StandardLogFormatter : public ILogFormatter {
public:
    StandardLogFormatter(bool include_context = true, bool include_trace = false);
    std::string format(const LogEntry& entry) const override;

private:
    bool include_context_;
    bool include_trace_;
    
    std::string levelToString(LogLevel level) const;
    std::string timestampToString(const std::chrono::system_clock::time_point& timestamp) const;
};

/**
 * @brief JSON log formatter
 * 
 * Formatter that produces structured JSON log output
 * for machine processing and compliance reporting.
 */
class JsonLogFormatter : public ILogFormatter {
public:
    JsonLogFormatter(bool pretty_print = false);
    std::string format(const LogEntry& entry) const override;

private:
    bool pretty_print_;
    
    std::string escapeJsonString(const std::string& str) const;
};

/**
 * @brief File output implementation
 * 
 * Thread-safe file output with rotation and compression support.
 */
class FileLogOutput : public ILogOutput {
public:
    FileLogOutput(const std::string& filename, size_t max_size_mb = 100, int max_files = 10);
    ~FileLogOutput();
    
    bool write(const std::string& formatted_entry) override;
    void flush() override;
    
    bool rotate();
    size_t getCurrentSize() const;

private:
    std::string filename_;
    std::unique_ptr<std::ofstream> file_;
    size_t max_size_bytes_;
    int max_files_;
    size_t current_size_;
    mutable std::mutex mutex_;
    
    void openFile();
    void closeFile();
    std::string getRotatedFilename(int index) const;
};

/**
 * @brief Console output implementation
 * 
 * Thread-safe console output with color coding support.
 */
class ConsoleLogOutput : public ILogOutput {
public:
    ConsoleLogOutput(bool enable_colors = true);
    
    bool write(const std::string& formatted_entry) override;
    void flush() override;

private:
    bool enable_colors_;
    mutable std::mutex mutex_;
    
    std::string colorize(const std::string& text, LogLevel level) const;
};

} // namespace core
} // namespace therapy_device

/**
 * @brief Convenience macros for logging
 * 
 * These macros provide convenient logging with automatic component
 * name detection and reduced overhead for disabled log levels.
 */
#define THERAPY_LOG_TRACE(message) \
    therapy_device::core::Logger::getInstance().trace(__FUNCTION__, message)

#define THERAPY_LOG_DEBUG(message) \
    therapy_device::core::Logger::getInstance().debug(__FUNCTION__, message)

#define THERAPY_LOG_INFO(message) \
    therapy_device::core::Logger::getInstance().info(__FUNCTION__, message)

#define THERAPY_LOG_WARNING(message) \
    therapy_device::core::Logger::getInstance().warning(__FUNCTION__, message)

#define THERAPY_LOG_ERROR(message) \
    therapy_device::core::Logger::getInstance().error(__FUNCTION__, message)

#define THERAPY_LOG_CRITICAL(message) \
    therapy_device::core::Logger::getInstance().critical(__FUNCTION__, message)

#define THERAPY_LOG_FATAL(message) \
    therapy_device::core::Logger::getInstance().fatal(__FUNCTION__, message)

// Context-aware logging macros
#define THERAPY_LOG_INFO_CTX(message, context) \
    therapy_device::core::Logger::getInstance().info(__FUNCTION__, message, context)

#define THERAPY_LOG_ERROR_CTX(message, context) \
    therapy_device::core::Logger::getInstance().error(__FUNCTION__, message, context)
