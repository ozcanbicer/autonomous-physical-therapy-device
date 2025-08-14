/**
 * @file config_manager.h
 * @brief Configuration management system for medical device compliance
 * 
 * Thread-safe configuration management with validation, encryption,
 * and audit trail support for IEC 62304 Class C compliance.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-CFG-001, REQ-CFG-002, REQ-CFG-003
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <chrono>
#include <atomic>
#include <variant>

namespace therapy_device {
namespace core {

/**
 * @brief Configuration value types
 * 
 * Supported configuration value types with type safety.
 */
using ConfigValue = std::variant<
    std::string,
    int,
    double,
    bool,
    std::vector<std::string>,
    std::vector<int>,
    std::vector<double>
>;

/**
 * @brief Configuration validation result
 * 
 * Result of configuration validation with detailed error information.
 */
struct ValidationResult {
    bool is_valid;
    std::vector<std::string> error_messages;
    std::vector<std::string> warning_messages;
    std::string validation_timestamp;
};

/**
 * @brief Configuration change event
 * 
 * Information about configuration changes for audit trail.
 */
struct ConfigChangeEvent {
    std::string key;
    ConfigValue old_value;
    ConfigValue new_value;
    std::string user_id;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    std::string change_reason;
    bool requires_restart;
};

/**
 * @brief Configuration section
 * 
 * Logical grouping of related configuration parameters.
 */
struct ConfigSection {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, std::string> validation_rules;
    bool is_encrypted;
    bool requires_medical_approval;
    std::string last_modified_by;
    std::chrono::system_clock::time_point last_modified_time;
};

/**
 * @brief Configuration validator interface
 * 
 * Interface for custom configuration validation implementations.
 */
class IConfigValidator {
public:
    virtual ~IConfigValidator() = default;
    
    /**
     * @brief Validate configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param context Validation context
     * @return Validation result
     */
    virtual ValidationResult validate(const std::string& key, 
                                    const ConfigValue& value,
                                    const std::map<std::string, ConfigValue>& context) const = 0;
};

/**
 * @brief Configuration change listener interface
 * 
 * Interface for receiving configuration change notifications.
 */
class IConfigChangeListener {
public:
    virtual ~IConfigChangeListener() = default;
    
    /**
     * @brief Handle configuration change
     * @param event Configuration change event
     */
    virtual void onConfigChanged(const ConfigChangeEvent& event) = 0;
};

/**
 * @brief Medical device configuration manager
 * 
 * Comprehensive configuration management system with medical device
 * compliance features including validation, encryption, audit trails,
 * and regulatory compliance support.
 * 
 * Key Features:
 * - IEC 62304 Class C compliance
 * - Thread-safe operation
 * - Configuration validation and constraints
 * - Encrypted storage for sensitive data
 * - Audit trail for all changes
 * - Hot-reload capability
 * - Schema validation
 * - Backup and restore
 * - Environment-specific configurations
 * 
 * Thread Safety: All public methods are thread-safe.
 * Security: Sensitive configurations are encrypted at rest.
 * Compliance: Full audit trail and change tracking.
 * 
 * Traceability: REQ-CFG-001 (Configuration management)
 */
class ConfigManager {
public:
    /**
     * @brief Get singleton configuration manager
     * @return Reference to singleton instance
     */
    static ConfigManager& getInstance();

    /**
     * @brief Configuration manager settings
     */
    struct Settings {
        std::string config_file_path = "config/device_config.json";
        std::string schema_file_path = "config/config_schema.json";
        std::string backup_directory = "config/backups";
        std::string encryption_key_file = "config/encryption.key";
        bool enable_encryption = true;
        bool enable_validation = true;
        bool enable_audit_trail = true;
        bool enable_hot_reload = true;
        bool enable_backup = true;
        int max_backup_count = 10;
        int auto_save_interval_seconds = 30;
        bool require_medical_approval = true;
        std::string environment = "production";  // "development", "testing", "production"
    };
    
    /**
     * @brief Initialize configuration manager
     * @param settings Manager settings
     * @return True if initialization successful
     */
    bool initialize(const Settings& settings);
    
    /**
     * @brief Get current settings
     * @return Current manager settings
     */
    Settings getSettings() const;

    /**
     * @brief Configuration loading and saving
     */
    
    /**
     * @brief Load configuration from file
     * @param config_path Path to configuration file
     * @return True if load successful
     */
    bool loadConfig(const std::string& config_path = "");
    
    /**
     * @brief Save configuration to file
     * @param config_path Path to configuration file (optional)
     * @return True if save successful
     */
    bool saveConfig(const std::string& config_path = "") const;
    
    /**
     * @brief Reload configuration from file
     * @return True if reload successful
     */
    bool reloadConfig();
    
    /**
     * @brief Load configuration schema
     * @param schema_path Path to schema file
     * @return True if schema load successful
     */
    bool loadSchema(const std::string& schema_path);

    /**
     * @brief Configuration access methods
     */
    
    /**
     * @brief Get configuration value
     * @param key Configuration key (dot-separated path)
     * @param default_value Default value if key not found
     * @return Configuration value or default
     */
    template<typename T>
    T getValue(const std::string& key, const T& default_value = T{}) const;
    
    /**
     * @brief Set configuration value
     * @param key Configuration key
     * @param value New value
     * @param user_id User making the change
     * @param reason Reason for change
     * @return True if set successful
     */
    template<typename T>
    bool setValue(const std::string& key, const T& value, 
                  const std::string& user_id = "system",
                  const std::string& reason = "");
    
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key
     * @return True if key exists
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * @param key Configuration key
     * @param user_id User making the change
     * @param reason Reason for removal
     * @return True if removal successful
     */
    bool removeKey(const std::string& key, 
                   const std::string& user_id = "system",
                   const std::string& reason = "");
    
    /**
     * @brief Get all keys in a section
     * @param section_prefix Section prefix (e.g., "camera.")
     * @return Vector of keys in section
     */
    std::vector<std::string> getKeysInSection(const std::string& section_prefix) const;
    
    /**
     * @brief Get configuration section
     * @param section_name Section name
     * @return Configuration section
     */
    ConfigSection getSection(const std::string& section_name) const;
    
    /**
     * @brief Set entire configuration section
     * @param section Configuration section
     * @param user_id User making the change
     * @param reason Reason for change
     * @return True if set successful
     */
    bool setSection(const ConfigSection& section, 
                    const std::string& user_id = "system",
                    const std::string& reason = "");

    /**
     * @brief Specialized getters for common types
     */
    std::string getString(const std::string& key, const std::string& default_value = "") const;
    int getInt(const std::string& key, int default_value = 0) const;
    double getDouble(const std::string& key, double default_value = 0.0) const;
    bool getBool(const std::string& key, bool default_value = false) const;
    std::vector<std::string> getStringArray(const std::string& key, 
                                          const std::vector<std::string>& default_value = {}) const;

    /**
     * @brief Validation and compliance
     */
    
    /**
     * @brief Validate entire configuration
     * @return Validation result
     */
    ValidationResult validateConfiguration() const;
    
    /**
     * @brief Validate specific configuration value
     * @param key Configuration key
     * @param value Value to validate
     * @return Validation result
     */
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief Add custom validator
     * @param key Configuration key pattern
     * @param validator Custom validator
     */
    void addValidator(const std::string& key, std::unique_ptr<IConfigValidator> validator);
    
    /**
     * @brief Remove custom validator
     * @param key Configuration key pattern
     */
    void removeValidator(const std::string& key);

    /**
     * @brief Change tracking and audit
     */
    
    /**
     * @brief Add change listener
     * @param listener Change listener
     */
    void addChangeListener(std::shared_ptr<IConfigChangeListener> listener);
    
    /**
     * @brief Remove change listener
     * @param listener Change listener to remove
     */
    void removeChangeListener(std::shared_ptr<IConfigChangeListener> listener);
    
    /**
     * @brief Get configuration change history
     * @param key Configuration key (optional, empty for all changes)
     * @param start_time Start time for history
     * @param end_time End time for history
     * @return Vector of change events
     */
    std::vector<ConfigChangeEvent> getChangeHistory(
        const std::string& key = "",
        const std::chrono::system_clock::time_point& start_time = {},
        const std::chrono::system_clock::time_point& end_time = {}) const;
    
    /**
     * @brief Generate audit report
     * @param start_time Start time for report
     * @param end_time End time for report
     * @return Audit report content
     */
    std::string generateAuditReport(
        const std::chrono::system_clock::time_point& start_time,
        const std::chrono::system_clock::time_point& end_time) const;

    /**
     * @brief Backup and restore
     */
    
    /**
     * @brief Create configuration backup
     * @param backup_name Backup name (optional)
     * @return True if backup successful
     */
    bool createBackup(const std::string& backup_name = "") const;
    
    /**
     * @brief Restore from backup
     * @param backup_name Backup name
     * @param user_id User performing restore
     * @return True if restore successful
     */
    bool restoreFromBackup(const std::string& backup_name, const std::string& user_id);
    
    /**
     * @brief List available backups
     * @return Vector of backup names with timestamps
     */
    std::vector<std::pair<std::string, std::chrono::system_clock::time_point>> listBackups() const;
    
    /**
     * @brief Delete old backups
     * @param keep_count Number of backups to keep
     * @return Number of backups deleted
     */
    int cleanupBackups(int keep_count = -1);

    /**
     * @brief Environment and deployment
     */
    
    /**
     * @brief Set environment
     * @param environment Environment name ("development", "testing", "production")
     */
    void setEnvironment(const std::string& environment);
    
    /**
     * @brief Get current environment
     * @return Current environment name
     */
    std::string getEnvironment() const;
    
    /**
     * @brief Load environment-specific configuration
     * @param environment Environment name
     * @return True if load successful
     */
    bool loadEnvironmentConfig(const std::string& environment);

    /**
     * @brief Medical device specific methods
     */
    
    /**
     * @brief Get medical device configuration
     * @return Medical device specific configuration
     */
    struct MedicalDeviceConfig {
        std::string device_id;
        std::string device_model;
        std::string firmware_version;
        std::string calibration_date;
        bool medical_mode_enabled;
        int max_session_duration_minutes;
        double safety_timeout_seconds;
        std::string regulatory_approval;
        std::vector<std::string> approved_users;
    } getMedicalDeviceConfig() const;
    
    /**
     * @brief Update medical device configuration
     * @param config New medical device configuration
     * @param user_id User making the change
     * @param approval_code Medical approval code
     * @return True if update successful
     */
    bool updateMedicalDeviceConfig(const MedicalDeviceConfig& config,
                                  const std::string& user_id,
                                  const std::string& approval_code);
    
    /**
     * @brief Validate medical compliance
     * @return True if configuration meets medical compliance requirements
     */
    bool validateMedicalCompliance() const;

    /**
     * @brief Status and diagnostics
     */
    
    /**
     * @brief Get configuration manager status
     * @return Status information
     */
    struct Status {
        bool is_initialized;
        bool is_valid;
        std::string config_file_path;
        std::chrono::system_clock::time_point last_loaded;
        std::chrono::system_clock::time_point last_saved;
        int total_keys;
        int encrypted_keys;
        int validation_errors;
        int pending_changes;
        bool auto_save_enabled;
        std::string environment;
    } getStatus() const;
    
    /**
     * @brief Get configuration statistics
     * @return Configuration usage statistics
     */
    struct Statistics {
        int total_reads;
        int total_writes;
        int validation_failures;
        int backup_count;
        int change_events;
        std::chrono::system_clock::time_point oldest_change;
        std::chrono::system_clock::time_point newest_change;
        std::map<std::string, int> section_access_counts;
    } getStatistics() const;

private:
    /**
     * @brief Private constructor for singleton
     */
    ConfigManager();
    
    /**
     * @brief Destructor
     */
    ~ConfigManager();
    
    /**
     * @brief Delete copy constructor and assignment
     */
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    /**
     * @brief Internal state
     */
    mutable std::recursive_mutex mutex_;
    Settings settings_;
    std::atomic<bool> initialized_;
    std::atomic<bool> auto_save_enabled_;
    
    /**
     * @brief Configuration data
     */
    std::map<std::string, ConfigSection> sections_;
    std::map<std::string, ConfigValue> flat_config_;
    std::map<std::string, std::string> config_schema_;
    
    /**
     * @brief Validation and listeners
     */
    std::map<std::string, std::unique_ptr<IConfigValidator>> validators_;
    std::vector<std::weak_ptr<IConfigChangeListener>> change_listeners_;
    
    /**
     * @brief Change tracking
     */
    std::vector<ConfigChangeEvent> change_history_;
    mutable Statistics statistics_;
    
    /**
     * @brief Auto-save thread
     */
    std::unique_ptr<std::thread> auto_save_thread_;
    std::atomic<bool> auto_save_running_;
    std::condition_variable auto_save_cv_;
    std::mutex auto_save_mutex_;

    /**
     * @brief Private helper methods
     */
    
    /**
     * @brief Parse configuration key path
     * @param key Dot-separated key path
     * @return Section name and key name pair
     */
    std::pair<std::string, std::string> parseKeyPath(const std::string& key) const;
    
    /**
     * @brief Flatten configuration sections to flat map
     */
    void flattenConfiguration();
    
    /**
     * @brief Rebuild sections from flat configuration
     */
    void rebuildSections();
    
    /**
     * @brief Encrypt sensitive configuration value
     * @param value Value to encrypt
     * @return Encrypted value
     */
    std::string encryptValue(const std::string& value) const;
    
    /**
     * @brief Decrypt configuration value
     * @param encrypted_value Encrypted value
     * @return Decrypted value
     */
    std::string decryptValue(const std::string& encrypted_value) const;
    
    /**
     * @brief Notify change listeners
     * @param event Change event
     */
    void notifyChangeListeners(const ConfigChangeEvent& event);
    
    /**
     * @brief Record configuration change
     * @param event Change event
     */
    void recordChange(const ConfigChangeEvent& event);
    
    /**
     * @brief Auto-save thread function
     */
    void autoSaveThreadFunction();
    
    /**
     * @brief Load encryption key
     * @return True if key loaded successfully
     */
    bool loadEncryptionKey();
    
    /**
     * @brief Generate backup filename
     * @param backup_name Optional backup name
     * @return Full backup file path
     */
    std::string generateBackupFilename(const std::string& backup_name = "") const;
    
    /**
     * @brief Update statistics
     * @param operation Operation type ("read", "write", "validate", etc.)
     * @param key Configuration key
     */
    void updateStatistics(const std::string& operation, const std::string& key = "") const;
};

/**
 * @brief Standard configuration validators
 */

/**
 * @brief Range validator for numeric values
 */
class RangeValidator : public IConfigValidator {
public:
    RangeValidator(double min_value, double max_value);
    ValidationResult validate(const std::string& key, const ConfigValue& value,
                            const std::map<std::string, ConfigValue>& context) const override;

private:
    double min_value_;
    double max_value_;
};

/**
 * @brief String pattern validator using regex
 */
class PatternValidator : public IConfigValidator {
public:
    PatternValidator(const std::string& pattern, const std::string& description = "");
    ValidationResult validate(const std::string& key, const ConfigValue& value,
                            const std::map<std::string, ConfigValue>& context) const override;

private:
    std::string pattern_;
    std::string description_;
};

/**
 * @brief Enum validator for string values
 */
class EnumValidator : public IConfigValidator {
public:
    EnumValidator(const std::vector<std::string>& allowed_values);
    ValidationResult validate(const std::string& key, const ConfigValue& value,
                            const std::map<std::string, ConfigValue>& context) const override;

private:
    std::vector<std::string> allowed_values_;
};

/**
 * @brief File path validator
 */
class FilePathValidator : public IConfigValidator {
public:
    FilePathValidator(bool must_exist = false, bool must_be_readable = false, bool must_be_writable = false);
    ValidationResult validate(const std::string& key, const ConfigValue& value,
                            const std::map<std::string, ConfigValue>& context) const override;

private:
    bool must_exist_;
    bool must_be_readable_;
    bool must_be_writable_;
};

} // namespace core
} // namespace therapy_device
