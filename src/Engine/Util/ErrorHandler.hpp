#pragma once

#include <string>
#include <functional>
#include <stdexcept>
#include <vector>
#include <mutex>

namespace error_handling {

// Exception types
class EngineException : public std::runtime_error {
public:
    EngineException(const std::string& message);
    virtual const char* what() const noexcept override;
};

class GraphicsException : public EngineException {
public:
    GraphicsException(const std::string& message);
};

class FileSystemException : public EngineException {
public:
    FileSystemException(const std::string& message);
};

class ResourceException : public EngineException {
public:
    ResourceException(const std::string& message);
};

// Callback types for error handling
enum class ErrorSeverity {
    Low,        // Non-critical issues
    Medium,     // Important issues that don't require immediate action
    High,       // Critical issues that may cause instability
    Fatal       // Issues that will cause the application to terminate
};

// Structure to hold error information
struct ErrorInfo {
    std::string message;
    std::string context;
    ErrorSeverity severity;
    std::string stackTrace;
    std::string subsystem;
};

// Error callback function type
using ErrorCallback = std::function<void(const ErrorInfo&)>;

// Register/unregister error callbacks
void registerErrorCallback(ErrorCallback callback);
void unregisterErrorCallback(ErrorCallback callback);
void clearErrorCallbacks();

// Signal handler setup
void setupSignalHandlers();

// Error checking functions
void checkGLError(const char* operation);
bool checkFileExists(const std::string& path);
void validateShader(unsigned int shader, const std::string& shaderType);
void validateProgram(unsigned int program);

// Helper for checking function results
template<typename T>
T checkResult(T result, const std::function<bool(T)>& validator, const std::string& errorMessage) {
    if (!validator(result)) {
        throw EngineException(errorMessage);
    }
    return result;
}

// Exception/error reporting
void reportException(const std::exception& e);
void reportFatalError(const std::string& message);
void reportGlError(const std::string& context);

// Report an error with specific severity
void reportError(const std::string& message, const std::string& context = "General", 
                 ErrorSeverity severity = ErrorSeverity::Medium,
                 const std::string& subsystem = "Engine");

} // namespace error_handling

// Shortcut macros for common error checks
#define CHECK_GL_ERROR(op) error_handling::checkGLError(#op)
#define VALIDATE_SHADER(shader, type) error_handling::validateShader(shader, type)
#define VALIDATE_PROGRAM(program) error_handling::validateProgram(program)
#define CHECK_FILE_EXISTS(path) error_handling::checkFileExists(path)

// Error handling macro for operations
#define ENGINE_TRY_CATCH(action, error_message) \
    try { \
        action; \
    } catch (const std::exception& e) { \
        error_handling::reportException(e); \
        LOG_ERROR(logging::gEngineLogger, "{}: {}", error_message, e.what()); \
    }

// New error reporting macros for different subsystems and severities
#define REPORT_ERROR(msg, ctx, sev, subsys) \
    error_handling::reportError(msg, ctx, sev, subsys)

#define ENGINE_ERROR(msg, ctx, sev) \
    REPORT_ERROR(msg, ctx, sev, "Engine")

#define GRAPHICS_ERROR(msg, ctx, sev) \
    REPORT_ERROR(msg, ctx, sev, "Graphics")

#define RESOURCE_ERROR(msg, ctx, sev) \
    REPORT_ERROR(msg, ctx, sev, "Resource")