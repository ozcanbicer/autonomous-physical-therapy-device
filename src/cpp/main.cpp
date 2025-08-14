/**
 * @file main.cpp
 * @brief Main entry point for Autonomous Physical Therapy Device
 * 
 * This is the main application entry point for the IEC 62304 Class C
 * medical device software. It initializes all system components and
 * manages the main application lifecycle.
 * 
 * @note This file is part of IEC 62304 Class C medical device software
 * @author Medical Device Team
 * @date 2025
 * @version 1.0.0
 * 
 * Traceability: REQ-SYS-001, REQ-SYS-002, REQ-SYS-003
 */

#include <iostream>
#include <memory>
#include <csignal>
#include <chrono>
#include <thread>

#include "config.h"
#include "core/application.h"
#include "core/logger.h"
#include "core/config_manager.h"
#include "core/error_handler.h"
#include "utils/command_line_parser.h"

using namespace therapy_device;

namespace {
    std::unique_ptr<core::Application> g_application;
    std::atomic<bool> g_shutdown_requested{false};
}

/**
 * @brief Signal handler for graceful shutdown
 * @param signal Signal number received
 * 
 * Handles SIGINT and SIGTERM signals to ensure graceful shutdown
 * of the medical device software.
 * 
 * Traceability: REQ-SYS-005 (Safety shutdown)
 */
void signalHandler(int signal) {
    auto& logger = core::Logger::getInstance();
    
    switch (signal) {
        case SIGINT:
            logger.log(core::LogLevel::INFO, "main", "Received SIGINT, initiating graceful shutdown");
            break;
        case SIGTERM:
            logger.log(core::LogLevel::INFO, "main", "Received SIGTERM, initiating graceful shutdown");
            break;
        default:
            logger.log(core::LogLevel::WARNING, "main", 
                      "Received unexpected signal: " + std::to_string(signal));
            break;
    }
    
    g_shutdown_requested.store(true);
    
    if (g_application) {
        g_application->requestShutdown();
    }
}

/**
 * @brief Initialize signal handlers
 * 
 * Sets up signal handlers for graceful shutdown in compliance
 * with medical device safety requirements.
 */
void initializeSignalHandlers() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Block SIGPIPE to handle network disconnections gracefully
    std::signal(SIGPIPE, SIG_IGN);
}

/**
 * @brief Print application information
 * 
 * Displays version information and build configuration for
 * traceability and debugging purposes.
 */
void printApplicationInfo() {
    std::cout << "=== Autonomous Physical Therapy Device ===" << std::endl;
    std::cout << "Version: " << config::PROJECT_VERSION << std::endl;
    std::cout << "Build Type: " << config::BUILD_TYPE << std::endl;
    std::cout << "Target Platform: " << config::TARGET_PLATFORM << std::endl;
    std::cout << "IEC 62304 Class C Compliant" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Hardware Support:" << std::endl;
    std::cout << "  Intel RealSense D435: " << (config::HAS_REALSENSE ? "YES" : "NO") << std::endl;
    std::cout << "  ORBBEC Femto Mega: " << (config::HAS_ORBBEC ? "YES" : "NO") << std::endl;
    std::cout << "  TensorRT Acceleration: " << (config::HAS_TENSORRT ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Software Features:" << std::endl;
    std::cout << "  MediaPipe AI: " << (config::HAS_MEDIAPIPE ? "YES" : "NO") << std::endl;
    std::cout << "  WebSocket Communication: " << (config::HAS_WEBSOCKET ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance Requirements:" << std::endl;
    std::cout << "  Max Latency: " << config::performance::MAX_LATENCY_MS << "ms" << std::endl;
    std::cout << "  Target FPS: " << config::performance::TARGET_FPS << std::endl;
    std::cout << "  Max Memory: " << config::performance::MAX_MEMORY_MB << "MB" << std::endl;
    std::cout << "  Max Power: " << config::performance::MAX_POWER_WATTS << "W" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;
}

/**
 * @brief Validate system requirements
 * @return true if system meets requirements, false otherwise
 * 
 * Performs system validation to ensure the platform meets
 * the minimum requirements for safe operation.
 * 
 * Traceability: REQ-SYS-004 (System validation)
 */
bool validateSystemRequirements() {
    auto& logger = core::Logger::getInstance();
    bool valid = true;
    
    logger.log(core::LogLevel::INFO, "main", "Validating system requirements");
    
    // Check if at least one camera is available
    if (!config::HAS_REALSENSE && !config::HAS_ORBBEC) {
        logger.log(core::LogLevel::ERROR, "main", 
                  "No supported cameras found (RealSense D435 or ORBBEC Femto Mega required)");
        valid = false;
    }
    
    // Validate memory requirements for Jetson
    if (config::IS_JETSON) {
        // TODO: Add actual memory check
        logger.log(core::LogLevel::INFO, "main", "Running on Jetson Orin NX platform");
    }
    
    // Validate AI processing capability
    if (!config::HAS_MEDIAPIPE) {
        logger.log(core::LogLevel::WARNING, "main", 
                  "MediaPipe not available - AI processing capabilities limited");
    }
    
    if (valid) {
        logger.log(core::LogLevel::INFO, "main", "System requirements validation passed");
    } else {
        logger.log(core::LogLevel::ERROR, "main", "System requirements validation failed");
    }
    
    return valid;
}

/**
 * @brief Main application entry point
 * @param argc Number of command line arguments
 * @param argv Command line argument array
 * @return Exit code (0 for success, non-zero for error)
 * 
 * Main entry point that initializes the medical device software,
 * validates system requirements, and starts the main application loop.
 * 
 * Traceability: REQ-SYS-001 (System initialization)
 */
int main(int argc, char* argv[]) {
    int exitCode = config::error_codes::SUCCESS;
    
    try {
        // Initialize signal handlers first for safety
        initializeSignalHandlers();
        
        // Parse command line arguments
        utils::CommandLineParser cmdParser;
        if (!cmdParser.parse(argc, argv)) {
            std::cerr << "Error: Invalid command line arguments" << std::endl;
            cmdParser.printUsage();
            return config::error_codes::CONFIGURATION_ERROR;
        }
        
        // Show help if requested
        if (cmdParser.hasOption("help")) {
            printApplicationInfo();
            cmdParser.printUsage();
            return config::error_codes::SUCCESS;
        }
        
        // Show version if requested
        if (cmdParser.hasOption("version")) {
            std::cout << config::PROJECT_NAME << " v" << config::PROJECT_VERSION << std::endl;
            return config::error_codes::SUCCESS;
        }
        
        // Print application information
        printApplicationInfo();
        
        // Initialize core systems
        auto& logger = core::Logger::getInstance();
        auto& configManager = core::ConfigManager::getInstance();
        auto& errorHandler = core::ErrorHandler::getInstance();
        
        // Configure logging
        logger.setLogLevel(core::LogLevel::INFO);
        logger.enableConsoleOutput(true);
        logger.enableFileOutput(true, "therapy_device.log");
        
        logger.log(core::LogLevel::INFO, "main", 
                  "Starting Autonomous Physical Therapy Device v" + std::string(config::PROJECT_VERSION));
        
        // Load configuration
        std::string configPath = cmdParser.getOption("config", "config/device_config.json");
        try {
            configManager.loadConfig(configPath);
            logger.log(core::LogLevel::INFO, "main", "Configuration loaded successfully");
        } catch (const std::exception& e) {
            logger.log(core::LogLevel::ERROR, "main", 
                      "Failed to load configuration: " + std::string(e.what()));
            return config::error_codes::CONFIGURATION_ERROR;
        }
        
        // Validate system requirements
        if (!validateSystemRequirements()) {
            logger.log(core::LogLevel::ERROR, "main", 
                      "System requirements validation failed - cannot start safely");
            return config::error_codes::HARDWARE_FAULT;
        }
        
        // Create and initialize the main application
        g_application = std::make_unique<core::Application>();
        
        if (!g_application->initialize(cmdParser)) {
            logger.log(core::LogLevel::ERROR, "main", "Application initialization failed");
            return config::error_codes::CAMERA_INIT_FAILED;
        }
        
        logger.log(core::LogLevel::INFO, "main", "Application initialized successfully");
        
        // Start the main application loop
        logger.log(core::LogLevel::INFO, "main", "Starting main application loop");
        
        exitCode = g_application->run();
        
        if (exitCode == config::error_codes::SUCCESS) {
            logger.log(core::LogLevel::INFO, "main", "Application completed successfully");
        } else {
            logger.log(core::LogLevel::ERROR, "main", 
                      "Application exited with error code: " + std::to_string(exitCode));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        
        if (g_application) {
            auto& logger = core::Logger::getInstance();
            logger.log(core::LogLevel::FATAL, "main", 
                      "Unhandled exception: " + std::string(e.what()));
        }
        
        exitCode = config::error_codes::SAFETY_VIOLATION;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception" << std::endl;
        exitCode = config::error_codes::SAFETY_VIOLATION;
    }
    
    // Cleanup
    if (g_application) {
        try {
            g_application->shutdown();
            auto& logger = core::Logger::getInstance();
            logger.log(core::LogLevel::INFO, "main", "Application shutdown completed");
        } catch (const std::exception& e) {
            std::cerr << "Error during shutdown: " << e.what() << std::endl;
        }
        
        g_application.reset();
    }
    
    // Final status message
    if (exitCode == config::error_codes::SUCCESS) {
        std::cout << "Autonomous Physical Therapy Device exited successfully" << std::endl;
    } else {
        std::cerr << "Autonomous Physical Therapy Device exited with error code: " 
                  << exitCode << std::endl;
    }
    
    return exitCode;
}
