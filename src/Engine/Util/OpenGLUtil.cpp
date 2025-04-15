#include "pch.hpp"
#include "OpenGLUtil.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include "Engine/Util/GameSettings.hpp"
#include "Engine/Util/Logging.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
namespace gl {
    namespace detail {
        bool g_GLDebugOutput = true;
        bool g_BreakOnError = true;
        bool g_ProfilingEnabled = true;
        std::unordered_map<std::string, timer::TimerQuery> g_TimerQueries;
    }

    void init()
    {
        LOG_INFO(logging::gGraphicsLogger, "Initializing OpenGL...");
        
        glfwSetErrorCallback([](int error, const char* description) {
            LOG_ERROR(logging::gGraphicsLogger, "GLFW Error {}: {}", error, description);
        });

        if (!glfwInit())
        {
            LOG_CRITICAL(logging::gGraphicsLogger, "Failed to initialize GLFW");
            exit(EXIT_FAILURE);
        }

#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* game_window = glfwCreateWindow(
            settings::window_width,
            settings::window_height,
            "Game Window",
            nullptr,
            nullptr
        );
        if (!game_window)
        {
            LOG_CRITICAL(logging::gGraphicsLogger, "Failed to create GLFW window");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(game_window);
        glfwSetKeyCallback(game_window, engine_util::keyCallback);
        glfwSetCursorPosCallback(game_window, engine_util::cursorPosCallback);
        glfwSetMouseButtonCallback(game_window, engine_util::mouseKeyCallback);
        glfwSetFramebufferSizeCallback(game_window, framebufferSizeCallback);
        glfwSwapInterval(0); // Disable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_CRITICAL(logging::gGraphicsLogger, "Failed to initialize GLAD");
            exit(EXIT_FAILURE);
        }

#ifdef _DEBUG
        if (GLAD_GL_VERSION_4_3)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(detail::debugMessageCallback, nullptr);

            // Configure debug output
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        }
#endif

        GL_CHECK(glClearColor(0, 0, 0, 0));
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        GL_CHECK(glEnable(GL_CULL_FACE));
        GL_CHECK(glCullFace(GL_BACK));
        GL_CHECK(glFrontFace(GL_CCW));
        GL_CHECK(glEnable(GL_BLEND));
        GL_CHECK(glBlendEquation(GL_FUNC_ADD));

        GameManager::setGLFWWindow(game_window);
        LOG_INFO(logging::gGraphicsLogger, "OpenGL initialized successfully");
    }

    void shutdown()
    {
        LOG_INFO(logging::gGraphicsLogger, "Shutting down OpenGL...");
        timer::reset();
        glfwDestroyWindow(GameManager::mGameWindow);
        glfwTerminate();
        LOG_INFO(logging::gGraphicsLogger, "OpenGL shutdown complete");
    }

    namespace timer {
        void begin(const char* name)
        {
#ifdef _DEBUG
            if (!detail::g_ProfilingEnabled) return;

            auto& timer = detail::g_TimerQueries[name];
            if (!timer.startQuery)
            {
                glGenQueries(1, &timer.startQuery);
                glGenQueries(1, &timer.endQuery);
                timer.minDuration = FLT_MAX;
                timer.maxDuration = -FLT_MAX;
                timer.totalDuration = 0;
                timer.sampleCount = 0;
            }
            timer.active = true;
            glQueryCounter(timer.startQuery, GL_TIMESTAMP);
#endif
        }

        void end(const char* name)
        {
#ifdef _DEBUG
            if (!detail::g_ProfilingEnabled) return;

            auto& timer = detail::g_TimerQueries[name];
            if (timer.active)
            {
                glQueryCounter(timer.endQuery, GL_TIMESTAMP);
                timer.active = false;

                GLint64 startTime, endTime;
                glGetQueryObjecti64v(timer.startQuery, GL_QUERY_RESULT, &startTime);
                glGetQueryObjecti64v(timer.endQuery, GL_QUERY_RESULT, &endTime);

                float duration = (endTime - startTime) / 1000000.0f;
                timer.lastDuration = duration;
                timer.minDuration = std::min(timer.minDuration, duration);
                timer.maxDuration = std::max(timer.maxDuration, duration);
                timer.totalDuration += duration;
                timer.sampleCount++;
            }
#endif
        }

        void reset()
        {
#ifdef _DEBUG
            for (auto& [name, timer] : detail::g_TimerQueries)
            {
                if (timer.startQuery)
                {
                    glDeleteQueries(1, &timer.startQuery);
                    glDeleteQueries(1, &timer.endQuery);
                }
            }
            detail::g_TimerQueries.clear();
#endif
        }

        void printResults()
        {
#ifdef _DEBUG
            if (!detail::g_ProfilingEnabled) return;

            for (const auto& [name, timer] : detail::g_TimerQueries)
            {
                if (timer.sampleCount > 0)
                {
                    float avgDuration = timer.totalDuration / timer.sampleCount;
                    std::cout << "GPU Operation '" << name << "' stats:\n"
                        << "  Average: " << avgDuration << "ms\n"
                        << "  Min: " << timer.minDuration << "ms\n"
                        << "  Max: " << timer.maxDuration << "ms\n"
                        << "  Samples: " << timer.sampleCount << std::endl;
                }
            }
#endif
        }

        void enableProfiling(bool enable) 
        { 
            detail::g_ProfilingEnabled = enable; 
        }

        bool isProfilingEnabled() 
        { 
            return detail::g_ProfilingEnabled; 
        }

        float getLastDuration(const char* name)
        {
#ifdef _DEBUG
            if (!detail::g_ProfilingEnabled) return 0.0f;
            auto it = detail::g_TimerQueries.find(name);
            if (it != detail::g_TimerQueries.end())
            {
                return it->second.lastDuration;
            }
#endif
            return 0.0f;
        }
    }

    void validateState(const char* location)
    {
#ifdef _DEBUG
        GLenum error;
        bool hasError = false;
        while ((error = glGetError()) != GL_NO_ERROR)
        {
            LOG_ERROR(logging::gGraphicsLogger, "OpenGL Error at {}: 0x{:x}", location, error);
            hasError = true;
        }

        if (hasError)
        {
            if(detail::g_BreakOnError) {
                ASSERT_LOG(logging::gGraphicsLogger, false, "OpenGL Error occurred! Check logs.");
            }
        }
#endif
    }

    void labelObject(GLenum type, GLuint object, const char* label)
    {
#ifdef _DEBUG
        if (GLAD_GL_VERSION_4_3)
        {
            //glObjectLabel(type, object, -1, label);
        }
#endif
    }

    void enableDebugOutput(bool enable)
    {
        detail::g_GLDebugOutput = enable;
#ifdef _DEBUG
        if (GLAD_GL_VERSION_4_3)
        {
            if (enable)
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            }
            else
            {
                glDisable(GL_DEBUG_OUTPUT);
                glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            }
        }
#endif
    }

    bool isDebugOutputEnabled()
    {
        return detail::g_GLDebugOutput;
    }

    void setBreakOnError(bool enable)
    {
        detail::g_BreakOnError = enable;
    }

    void beginFrame()
    {
#ifdef _DEBUG
        timer::reset();
#endif
    }

    void endFrame()
    {
#ifdef _DEBUG
        glFinish();
        //timer::printResults();
#endif
    }

    void drawQuad()
    {
        GL_SCOPED_MARKER("DrawQuad");
        static GLuint quadVAO = 0;
        static GLuint quadVBO = 0;
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
            };
            GL_CHECK(glGenVertexArrays(1, &quadVAO));
            GL_CHECK(glGenBuffers(1, &quadVBO));
            GL_CHECK(glBindVertexArray(quadVAO));
            GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, quadVBO));
            GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),
                                  quadVertices, GL_STATIC_DRAW));

            GL_CHECK(glEnableVertexAttribArray(0));
            GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                           4 * sizeof(float), (void*)0));

            GL_CHECK(glEnableVertexAttribArray(1));
            GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                                           4 * sizeof(float), (void*)(2 * sizeof(float))));
        }
        GL_CHECK(glBindVertexArray(quadVAO));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
        GL_CHECK(glBindVertexArray(0));
    }

void gl::drawCube()
{
    GL_SCOPED_MARKER("DrawCube");
    static unsigned int cubeVAO = 0;
    static unsigned int cubeVBO = 0;

    // Save GL state
    GLint last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // positions          // texture coords
            // Back face
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,

            // Left face
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

            // Right face
            1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
            1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
            1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
            1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

            // Front face
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
            -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
            1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
            1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
            1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

            // Top face
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
            1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
            1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,

            // Bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
            1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f,  1.0f, 0.0f
        };
        GL_CHECK(glGenVertexArrays(1, &cubeVAO));
        GL_CHECK(glGenBuffers(1, &cubeVBO));

        GL_CHECK(glBindVertexArray(cubeVAO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                              vertices, GL_STATIC_DRAW));

        // Position attribute
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                       5 * sizeof(float), (void*)0));

        // Texture coord attribute
        GL_CHECK(glEnableVertexAttribArray(1));
        GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                                       5 * sizeof(float), (void*)(3 * sizeof(float))));

        // Unbind VAO first, then VBO
        GL_CHECK(glBindVertexArray(0));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    // Draw
    GL_CHECK(glBindVertexArray(cubeVAO));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));

    // Restore state
    GL_CHECK(glBindVertexArray(last_vertex_array));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer));
}

void gl::checkGLState()
{
    GL_SCOPED_MARKER("CheckGLState");
    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    GLint activeProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgram);
    std::cout << "Active Shader Program: " << activeProgram << "\n";

    GLint activeTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    std::cout << "Active Texture Unit: " << (activeTexture - GL_TEXTURE0) << "\n";

    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    std::cout << "Blend Enabled: " << (blendEnabled ? "true" : "false") << "\n";

    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    std::cout << "Depth Test Enabled: " << (depthTestEnabled ? "true" : "false") << "\n";

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    std::cout << "Viewport: [" << viewport[0] << ", " << viewport[1] << ", " 
        << viewport[2] << ", " << viewport[3] << "]\n";

    // Check all texture units
    std::cout << "\nTexture Units State:\n";
    for(int i = 0; i < maxTextureUnits; i++) {
        checkTextureState(i);
    }

    GLint boundFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);
    std::cout << "\nBound Framebuffer: " << boundFBO << "\n";
}

void gl::checkTextureState(unsigned int unit)
{
    GL_SCOPED_MARKER("CheckTextureState");
    GLint activeUnit;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
    activeUnit -= GL_TEXTURE0;

    GLint boundTexture;
    glActiveTexture(GL_TEXTURE0 + unit);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);

    GLint compareMode;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, &compareMode);

    GLint compareFunc;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, &compareFunc);

    std::cout << "Texture Unit " << unit << " State:\n"
        << "  Bound Texture: " << boundTexture << "\n"
        << "  Compare Mode: " << (compareMode == GL_COMPARE_REF_TO_TEXTURE ? "GL_COMPARE_REF_TO_TEXTURE" : "GL_NONE") << "\n"
        << "  Compare Func: " << compareFunc << "\n";

    // Restore original active texture unit
    glActiveTexture(GL_TEXTURE0 + activeUnit);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void gl::clearBackground()
{
    GL_SCOPED_MARKER("ClearBackground");
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void APIENTRY gl::detail::debugMessageCallback(GLenum source, GLenum type, GLuint id,
                                               GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    // Ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    // Filter out notification severity unless explicitly enabled
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && !g_GLDebugOutput) return;

    std::string severityStr;
    spdlog::level::level_enum logLevel = spdlog::level::info;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: 
        severityStr = "HIGH"; 
        logLevel = spdlog::level::critical; 
        break;
    case GL_DEBUG_SEVERITY_MEDIUM: 
        severityStr = "MEDIUM"; 
        logLevel = spdlog::level::warn; 
        break;
    case GL_DEBUG_SEVERITY_LOW: 
        severityStr = "LOW"; 
        logLevel = spdlog::level::debug; 
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: 
        severityStr = "NOTIFICATION"; 
        logLevel = spdlog::level::trace; 
        break;
    default: 
        severityStr = "UNKNOWN"; 
        logLevel = spdlog::level::info;
        break;
    }

    logging::gGraphicsLogger->log(logLevel, "OpenGL Debug [{}]: ({}) {}", severityStr, id, message);

    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
         if(g_BreakOnError) {
            ASSERT_LOG(logging::gGraphicsLogger, false, "High severity OpenGL error occurred! Check logs.");
         }
    }
}
// Define the ScopedTimer and ScopedLabel implementations
gl::ScopedTimer::ScopedTimer(const char* name) : mName(name)
{
#ifdef _DEBUG
    timer::begin(name);
#endif
}

gl::ScopedTimer::~ScopedTimer()
{
#ifdef _DEBUG
    timer::end(mName);
#endif
}

gl::ScopedLabel::ScopedLabel(const char* label)
{
#ifdef _DEBUG
    if (GLAD_GL_VERSION_4_3)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, label);
    }
#endif
}

gl::ScopedLabel::~ScopedLabel()
{
#ifdef _DEBUG
    if (GLAD_GL_VERSION_4_3)
    {
        glPopDebugGroup();
    }
#endif
}
} // namespace gl


