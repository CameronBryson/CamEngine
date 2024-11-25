#pragma once
#include <string>
#include <vector>
#include "Engine/Util/platform.hpp"

#include <glm/fwd.hpp>
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
	static void drawLine(glm::vec2& start, glm::vec2& end, const glm::vec3& color);

	static void drawText(const char* text, int posX, int posY, int fontSize, glm::vec3& color);

	static void clearBackground();

	static glm::vec2 convertPointToScreen(glm::vec2& point);

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	static glm::vec2 getWindowSize();

	static glm::vec2 getMousePos();

	static void enableBlend();

	static void disableBlend();


};


