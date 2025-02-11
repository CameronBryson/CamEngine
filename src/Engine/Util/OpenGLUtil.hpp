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

	static void clearBackground();
	static void drawQuad();
	static void drawCube();


	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);



};


