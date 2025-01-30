#pragma once
#include "Cubemap.hpp"
#include "platform.hpp"
class VertexArray;
class OpenGLCubemap : public Cubemap
{
public:

	OpenGLCubemap(const std::string& facePaths);
	~OpenGLCubemap();
	void draw(const Shader& shader) const override;
	int getID() const override { return cubemapID; }
	int getIrradianceMapID() const override { return irradianceMapID; }
	int getPrefilterMapID() const override { return prefilteredMapID; }
	int getBRDFLUT() const override { return brdfLUTID; }
private:
	void createSkyboxVAO();
	int width, height;
	GLuint cubemapID;
	GLuint irradianceMapID;
	GLuint prefilteredMapID;
	GLuint brdfLUTID;

	GLuint vao, vbo;
};

