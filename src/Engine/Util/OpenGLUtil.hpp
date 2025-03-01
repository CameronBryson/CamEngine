#pragma once
#include <string>
#include <vector>
#include "Engine/Util/platform.hpp"
#include <iostream>
#include <glm/fwd.hpp>
#include <unordered_map>

// Macro definitions
#define GL_CHECK(stmt)                      \
    do {                                    \
        stmt;                               \
        GLenum err;                         \
        while ((err = glGetError()) != GL_NO_ERROR) {               \
            std::cerr << "[OpenGL Error] 0x" << std::hex << err     \
                      << " at " << __FILE__ << ":" << std::dec << __LINE__ \
                      << " for call: " << #stmt << std::endl;       \
        }                                  \
    } while (0)

namespace gl {

    // Forward declarations
    struct TimerQuery;
    class ScopedLabel;
    class ScopedTimer;

    // Core OpenGL utility functions
    void init();
    void shutdown();
    void beginFrame();
    void endFrame();
    void clearBackground();
    void drawQuad();
    void drawCube();
    void checkGLState();
    void checkTextureState(unsigned int unit);
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    // Debug features
    void enableDebugOutput(bool enable);
    bool isDebugOutputEnabled();
    void setBreakOnError(bool enable);
    void labelObject(GLenum type, GLuint object, const char* label);
    void validateState(const char* location);

    // GPU timing functionality
    namespace timer {
        void begin(const char* name);
        void end(const char* name);
        void reset();
        void printResults();
        void enableProfiling(bool enable);
        bool isProfilingEnabled();
        float getLastDuration(const char* name);

        struct TimerQuery {
            GLuint startQuery;
            GLuint endQuery;
            bool active;
            float lastDuration;
            float minDuration;
            float maxDuration;
            float totalDuration;
            uint32_t sampleCount;
        };
    }
    

    // Nested class definitions
    class ScopedTimer {
    public:
        explicit ScopedTimer(const char* name);
        ~ScopedTimer();

    private:
        const char* mName;
    };

    class ScopedLabel {
    public:
        explicit ScopedLabel(const char* label);
        ~ScopedLabel();
    };

    // Private/internal variables and functions
    namespace detail {
        extern bool g_GLDebugOutput;
        extern bool g_BreakOnError;
        extern std::unordered_map<std::string, timer::TimerQuery> g_TimerQueries;
        extern bool g_ProfilingEnabled;

        void APIENTRY debugMessageCallback(
            GLenum source, GLenum type, GLuint id, GLenum severity, 
            GLsizei length, const GLchar* message, const void* userParam);
    }

} // namespace gl

  // Debug macros
#ifdef _DEBUG
#define GL_SCOPED_MARKER(name) gl::ScopedLabel scopedLabel##__LINE__(name)
#define GL_SCOPED_TIMER(name) gl::ScopedTimer scopedTimer##__LINE__(name)
#define GL_LABEL_OBJECT(type, obj, name) gl::labelObject(type, obj, name)
#define GL_VALIDATE_STATE() gl::validateState(__FUNCTION__)
#else
#define GL_SCOPED_MARKER(name)
#define GL_SCOPED_TIMER(name)
#define GL_LABEL_OBJECT(type, obj, name)
#define GL_VALIDATE_STATE()
#endif
