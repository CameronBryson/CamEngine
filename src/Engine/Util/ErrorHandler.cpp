#include "pch.hpp"
#include "ErrorHandler.hpp"
#include "Logging.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <filesystem>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#include <unistd.h>
#endif

namespace error_handling {

// Exception implementations
EngineException::EngineException(const std::string& message)
    : std::runtime_error(message) {
    LOG_ERROR(logging::gEngineLogger, "Engine Exception: {}", message);
}

const char* EngineException::what() const noexcept {
    return std::runtime_error::what();
}

GraphicsException::GraphicsException(const std::string& message)
    : EngineException(message) {
    LOG_ERROR(logging::gGraphicsLogger, "Graphics Exception: {}", message);
}

FileSystemException::FileSystemException(const std::string& message)
    : EngineException(message) {
    LOG_ERROR(logging::gEngineLogger, "File System Exception: {}", message);
}

ResourceException::ResourceException(const std::string& message)
    : EngineException(message) {
    LOG_ERROR(logging::gResourceLogger, "Resource Exception: {}", message);
}

// Generate a stack trace
static std::string getStackTrace() {
    std::string result;

#ifdef _WIN32
    // Windows implementation
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    
    CONTEXT context;
    memset(&context, 0, sizeof(CONTEXT));
    context.ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(&context);

    SymInitialize(process, NULL, TRUE);
    
    DWORD image;
    STACKFRAME64 stack;
    ZeroMemory(&stack, sizeof(STACKFRAME64));

#ifdef _M_X64
    image = IMAGE_FILE_MACHINE_AMD64;
    stack.AddrPC.Offset = context.Rip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = context.Rbp;
    stack.AddrFrame.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context.Rsp;
    stack.AddrStack.Mode = AddrModeFlat;
#else
    image = IMAGE_FILE_MACHINE_I386;
    stack.AddrPC.Offset = context.Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = context.Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context.Esp;
    stack.AddrStack.Mode = AddrModeFlat;
#endif

    const int MAX_CALLERS = 62;
    result = "Stack trace:\n";
    
    for (int i = 0; i < MAX_CALLERS; i++) {
        BOOL success = StackWalk64(
            image, process, thread, &stack, &context,
            NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL);
        
        if (!success || stack.AddrPC.Offset == 0) {
            break;
        }
        
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        DWORD64 displacement = 0;
        DWORD lineDisplacement = 0;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (SymFromAddr(process, stack.AddrPC.Offset, &displacement, symbol)) {
            result += "    " + std::string(symbol->Name) + " ";
            
            if (SymGetLineFromAddr64(process, stack.AddrPC.Offset, &lineDisplacement, &line)) {
                result += "at " + std::string(line.FileName) + ":" + std::to_string(line.LineNumber);
            }
            
            result += "\n";
        } else {
            result += "    [unknown]\n";
        }
    }
    
    SymCleanup(process);
#else
    // Unix implementation
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    
    result = "Stack trace:\n";
    for (int i = 0; i < frames; ++i) {
        result += "    " + std::string(strs[i]) + "\n";
    }
    
    free(strs);
#endif

    return result;
}

// Signal handler function
static void signalHandler(int signal) {
    std::string signalName;
    switch (signal) {
        case SIGABRT: signalName = "SIGABRT (Abort)"; break;
        case SIGFPE:  signalName = "SIGFPE (Floating Point Exception)"; break;
        case SIGILL:  signalName = "SIGILL (Illegal Instruction)"; break;
        case SIGINT:  signalName = "SIGINT (Interrupt)"; break;
        case SIGSEGV: signalName = "SIGSEGV (Segmentation Violation)"; break;
        case SIGTERM: signalName = "SIGTERM (Terminate)"; break;
        default:      signalName = "Signal " + std::to_string(signal); break;
    }
    
    std::string errorMessage = "Fatal error: " + signalName + " received";
    std::string stackTrace = getStackTrace();
    
    // Log the error and stack trace
    if (logging::gEngineLogger) {
        LOG_CRITICAL(logging::gEngineLogger, "{}", errorMessage);
        LOG_CRITICAL(logging::gEngineLogger, "{}", stackTrace);
        spdlog::shutdown(); // Ensure logs are flushed
    } else {
        // If logger is not available, write to stderr
        std::cerr << errorMessage << std::endl;
        std::cerr << stackTrace << std::endl;
    }
    
    // Terminate the program
    std::exit(EXIT_FAILURE);
}

void setupSignalHandlers() {
    LOG_INFO(logging::gEngineLogger, "Setting up signal handlers for crash reporting");
    
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGFPE, signalHandler);
    std::signal(SIGILL, signalHandler);
    std::signal(SIGINT, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

void checkGLError(const char* operation) {
    GLenum error;
    bool hasError = false;
    std::string errorMsg;
    
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::string errorStr;
        switch (error) {
            case GL_INVALID_ENUM:      errorStr = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE:     errorStr = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:    errorStr = "GL_STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:   errorStr = "GL_STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:     errorStr = "GL_OUT_OF_MEMORY"; break;
            default:                  errorStr = "Unknown error code " + std::to_string(error); break;
        }
        
        if (!hasError) {
            errorMsg = "OpenGL error(s) after '" + std::string(operation) + "': " + errorStr;
            hasError = true;
        } else {
            errorMsg += ", " + errorStr;
        }
    }
    
    if (hasError) {
        LOG_ERROR(logging::gGraphicsLogger, "{}", errorMsg);
#ifdef _DEBUG
        throw GraphicsException(errorMsg);
#endif
    }
}

bool checkFileExists(const std::string& path) {
    bool exists = std::filesystem::exists(path);
    if (!exists) {
        LOG_ERROR(logging::gResourceLogger, "File not found: {}", path);
    }
    return exists;
}

void validateShader(unsigned int shader, const std::string& shaderType) {
    int success;
    char infoLog[1024];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::string errorMsg = "Shader compilation error in " + shaderType + " shader: " + infoLog;
        LOG_ERROR(logging::gGraphicsLogger, "{}", errorMsg);
        throw GraphicsException(errorMsg);
    }
}

void validateProgram(unsigned int program) {
    int success;
    char infoLog[1024];
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        std::string errorMsg = "Shader program linking error: " + std::string(infoLog);
        LOG_ERROR(logging::gGraphicsLogger, "{}", errorMsg);
        throw GraphicsException(errorMsg);
    }
}

void reportException(const std::exception& e) {
    LOG_ERROR(logging::gEngineLogger, "Exception: {}", e.what());
}

void reportFatalError(const std::string& message) {
    std::string errorMessage = "Fatal error: " + message;
    std::string stackTrace = getStackTrace();
    
    // Log the error and stack trace
    if (logging::gEngineLogger) {
        LOG_CRITICAL(logging::gEngineLogger, "{}", errorMessage);
        LOG_CRITICAL(logging::gEngineLogger, "{}", stackTrace);
        spdlog::shutdown(); // Ensure logs are flushed
    } else {
        // If logger is not available, write to stderr
        std::cerr << errorMessage << std::endl;
        std::cerr << stackTrace << std::endl;
    }
    
    // Terminate the program
    std::exit(EXIT_FAILURE);
}

void reportGlError(const std::string& context) {
    // Check for OpenGL errors
    GLenum error;
    bool hasError = false;
    std::string errorMsg = "OpenGL errors in context '" + context + "': ";
    
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::string errorStr;
        switch (error) {
            case GL_INVALID_ENUM:      errorStr = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE:     errorStr = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:    errorStr = "GL_STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:   errorStr = "GL_STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:     errorStr = "GL_OUT_OF_MEMORY"; break;
            default:                   errorStr = "Unknown error code " + std::to_string(error); break;
        }
        
        if (hasError) {
            errorMsg += ", " + errorStr;
        } else {
            errorMsg += errorStr;
            hasError = true;
        }
    }
    
    if (hasError) {
        LOG_ERROR(logging::gGraphicsLogger, "{}", errorMsg);
    }
}

} // namespace error_handling 