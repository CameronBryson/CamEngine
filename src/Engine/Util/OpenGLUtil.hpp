#pragma once
#include <string>
#include <vector>
#include "Engine/Util/platform.hpp"
#include <iostream>
#include <glm/fwd.hpp>
#include <unordered_map>
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
class OpenGlUtil {
public:
    // Add perfmarker management
    class GPUTimer {

    public:
        class ScopedTimer {
        public:
            explicit ScopedTimer(const char* name) : mName(name) {
#ifdef _DEBUG
                GPUTimer::begin(name);
#endif
            }

            ~ScopedTimer() {
#ifdef _DEBUG
                GPUTimer::end(mName);
#endif
            }

        private:
            const char* mName;
        };
        static void begin(const char* name);
        static void end(const char* name);
        static void reset();
        static void printResults();
        static void enableProfiling(bool enable) { sProfilingEnabled = enable; }
        static bool isProfilingEnabled() { return sProfilingEnabled; }
        static float getLastDuration(const char* name) {
#ifdef _DEBUG
            if (!sProfilingEnabled) return 0.0f;
            auto it = mTimerQueries.find(name);
            if (it != mTimerQueries.end()) {
                return it->second.lastDuration;
            }
#endif
            return 0.0f;
        }


    private:
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
        static std::unordered_map<std::string, TimerQuery> mTimerQueries;
        static bool sProfilingEnabled;
    };

    class ScopedLabel {
    public:
        explicit ScopedLabel(const char* label) {
#ifdef _DEBUG
            if (GLAD_GL_VERSION_4_3) {
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, label);
            }
#endif
        }
        ~ScopedLabel() {
#ifdef _DEBUG
            if (GLAD_GL_VERSION_4_3) {
                glPopDebugGroup();
            }
#endif
        }
    };

    static void init();
    static void shutdown();
    static void beginFrame();
    static void endFrame();
    static void clearBackground();
    static void drawQuad();
    static void drawCube();
    static void checkGLState();
    static void checkTextureState(unsigned int unit);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    // Debug features
    static void enableDebugOutput(bool enable);
    static bool isDebugOutputEnabled() { return sGLDebugOutput; }
    static void setBreakOnError(bool enable) { sBreakOnError = enable; }
    static void labelObject(GLenum type, GLuint object, const char* label);
    static void validateState(const char* location);

private:
    static bool sGLDebugOutput;
    static bool sBreakOnError;
    static void APIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id,
                                              GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};

// Debug macros
#ifdef _DEBUG
#define GL_SCOPED_MARKER(name) OpenGlUtil::ScopedLabel scopedLabel##__LINE__(name)
#define GL_SCOPED_TIMER(name) OpenGlUtil::GPUTimer::ScopedTimer scopedTimer##__LINE__(name)
#define GL_LABEL_OBJECT(type, obj, name) OpenGlUtil::labelObject(type, obj, name)
#define GL_VALIDATE_STATE() OpenGlUtil::validateState(__FUNCTION__)
#else
#define GL_SCOPED_MARKER(name)
#define GL_SCOPED_TIMER(name)
#define GL_LABEL_OBJECT(type, obj, name)
#define GL_VALIDATE_STATE()
#endif
