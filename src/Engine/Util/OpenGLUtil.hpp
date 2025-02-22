#pragma once
#include <string>
#include <vector>
#include "Engine/Util/platform.hpp"
#include <iostream>
#include <glm/fwd.hpp>

#ifdef _DEBUG
// Debug message callback that filters out low-severity messages
static void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    // Only print High and Medium severity
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        std::cerr << "[OpenGL Debug] ID: " << id
            << " Severity: " << severity
            << " Type: " << type
            << std::endl
            << "  Message: " << message << std::endl;
    }
}
#endif

// Simple macro to check OpenGL errors
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
struct Vertex;
class Material;
class Texture;
class Shader;
class GraphicsManager;

class OpenGlUtil
{
public:
	static void init();
	static void shutdown();

	static void clearBackground();
	static void drawQuad();
	static void drawCube();
    static void checkGLState();
    static void chechTextureState(unsigned int unit);


	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);



};


