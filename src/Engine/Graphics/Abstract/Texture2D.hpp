#pragma once
#include "Texture.hpp"
#include <memory>
#include <assimp/material.h>
#include <string>
#include <assimp/texture.h>
#include "platform.hpp"
class Texture2D : public Texture
{
public:
	static std::shared_ptr<Texture2D> createTexture2D(const std::string& file);
	static std::shared_ptr<Texture2D> createTexture2D(const aiTexture* aiTex);
	static std::shared_ptr<Texture2D> createTexture2D(GLuint textureID, int width, int height);
};

