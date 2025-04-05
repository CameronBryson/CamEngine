#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/cfg/env.h>
#include <memory>
#include <string>

namespace logging {

// Log level enum - mirrors spdlog's levels for convenience
enum class LogLevel {
    Trace = spdlog::level::trace,
    Debug = spdlog::level::debug,
    Info = spdlog::level::info,
    Warn = spdlog::level::warn,
    Error = spdlog::level::err,
    Critical = spdlog::level::critical,
    Off = spdlog::level::off
};

// Initialize the logging system
void init();

// Initialize the logging system with a specific global level
void init(LogLevel level);

// Set the global log level
void setGlobalLevel(LogLevel level);

// Set a specific logger's level
void setLoggerLevel(const std::string& loggerName, LogLevel level);

// Get the current global log level
LogLevel getGlobalLevel();

// Get a specific logger's level
LogLevel getLoggerLevel(const std::string& loggerName);

// Load log levels from environment variables
// Example: SPDLOG_LEVEL=info,graphics=debug,engine=trace
void loadEnvLevels();

// Create a new rotating file logger
std::shared_ptr<spdlog::logger> createRotatingLogger(
    const std::string& name, 
    const std::string& filename, 
    size_t max_size = 1048576 * 5,  // 5MB
    size_t max_files = 3);

// Flush all loggers
void flushAll();

// Shutdown the logging system (ensure all messages are flushed)
void shutdown();

// Get a logger for a specific subsystem
std::shared_ptr<spdlog::logger> getLogger(const std::string& name);

// Convenience macros for logging
#define LOG_TRACE(logger, ...) SPDLOG_LOGGER_TRACE(logger, __VA_ARGS__)
#define LOG_DEBUG(logger, ...) SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
#define LOG_INFO(logger, ...) SPDLOG_LOGGER_INFO(logger, __VA_ARGS__)
#define LOG_WARN(logger, ...) SPDLOG_LOGGER_WARN(logger, __VA_ARGS__)
#define LOG_ERROR(logger, ...) SPDLOG_LOGGER_ERROR(logger, __VA_ARGS__)
#define LOG_CRITICAL(logger, ...) SPDLOG_LOGGER_CRITICAL(logger, __VA_ARGS__)

// Global loggers for different subsystems
extern std::shared_ptr<spdlog::logger> gEngineLogger;
extern std::shared_ptr<spdlog::logger> gGraphicsLogger;
extern std::shared_ptr<spdlog::logger> gResourceLogger;

} // namespace logging 