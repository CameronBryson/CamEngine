#pragma once
#include "Cubemap.hpp"
#include "platform.hpp"
class VertexArray;
class OpenGLCubemap : public Cubemap
{
public:
	OpenGLCubemap(const std::vector<std::string> facePaths);
	~OpenGLCubemap();
	void draw(const Shader& shader) const override;
	int getID() const override { return cubemapID; }
private:
	void createSkyboxVAO();
	GLuint vao;
	GLuint vbo;
	GLuint cubemapID;
	int width;
	int height;
};

