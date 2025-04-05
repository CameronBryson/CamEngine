#include "pch.hpp"
#include "Logging.hpp"
#include <filesystem>
#include <iostream>

namespace logging {

// Global loggers
std::shared_ptr<spdlog::logger> gEngineLogger;
std::shared_ptr<spdlog::logger> gGraphicsLogger;
std::shared_ptr<spdlog::logger> gResourceLogger;

void init() {
    init(LogLevel::Info);
}

void init(LogLevel level) {
    try {
        // Create logs directory if it doesn't exist
        std::filesystem::create_directory("logs");

        // Create console sink with color
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");

        // Create file sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/engine.log", true);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");

        // Create rotating file sink (5MB max size, 3 files)
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/engine_rotating.log", 1048576 * 5, 3);
        rotating_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");

        // Create loggers with all sinks
        gEngineLogger = std::make_shared<spdlog::logger>("engine", 
            spdlog::sinks_init_list{console_sink, file_sink, rotating_sink});
        gGraphicsLogger = std::make_shared<spdlog::logger>("graphics", 
            spdlog::sinks_init_list{console_sink, file_sink, rotating_sink});
        gResourceLogger = std::make_shared<spdlog::logger>("resource", 
            spdlog::sinks_init_list{console_sink, file_sink, rotating_sink});

        // Set default log level based on build configuration or parameter
        setGlobalLevel(level);

        // Register loggers
        spdlog::register_logger(gEngineLogger);
        spdlog::register_logger(gGraphicsLogger);
        spdlog::register_logger(gResourceLogger);

        // Set flush level
        spdlog::flush_on(spdlog::level::err);

        // Try to load log levels from environment variables
        loadEnvLevels();

        LOG_INFO(gEngineLogger, "Logging system initialized");
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing logging system: " << e.what() << std::endl;
    }
}

void setGlobalLevel(LogLevel level) {
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
}

void setLoggerLevel(const std::string& loggerName, LogLevel level) {
    auto logger = spdlog::get(loggerName);
    if (logger) {
        logger->set_level(static_cast<spdlog::level::level_enum>(level));
    }
}

LogLevel getGlobalLevel() {
    return static_cast<LogLevel>(spdlog::get_level());
}

LogLevel getLoggerLevel(const std::string& loggerName) {
    auto logger = spdlog::get(loggerName);
    if (logger) {
        return static_cast<LogLevel>(logger->level());
    }
    return LogLevel::Off;
}

void loadEnvLevels() {
    try {
        spdlog::cfg::load_env_levels();
        LOG_DEBUG(gEngineLogger, "Loaded log levels from environment variables");
    }
    catch (const std::exception& e) {
        LOG_WARN(gEngineLogger, "Failed to load log levels from environment: {}", e.what());
    }
}

std::shared_ptr<spdlog::logger> createRotatingLogger(
    const std::string& name, 
    const std::string& filename, 
    size_t max_size, 
    size_t max_files) 
{
    try {
        auto logger = spdlog::rotating_logger_mt(name, filename, max_size, max_files);
        LOG_DEBUG(gEngineLogger, "Created rotating logger '{}' to file {}", name, filename);
        return logger;
    }
    catch (const std::exception& e) {
        LOG_ERROR(gEngineLogger, "Failed to create rotating logger '{}': {}", name, e.what());
        return nullptr;
    }
}

void flushAll() {
    spdlog::apply_all([](std::shared_ptr<spdlog::logger> l) { l->flush(); });
}

void shutdown() {
    LOG_INFO(gEngineLogger, "Shutting down logging system");
    flushAll();
    spdlog::shutdown();
}

std::shared_ptr<spdlog::logger> getLogger(const std::string& name) {
    auto logger = spdlog::get(name);
    if (!logger) {
        // Create a new logger if it doesn't exist
        logger = std::make_shared<spdlog::logger>(name, spdlog::sinks_init_list{
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/" + name + ".log", true)
        });
        spdlog::register_logger(logger);
        LOG_DEBUG(gEngineLogger, "Created new logger: {}", name);
    }
    return logger;
}

} // namespace logging 