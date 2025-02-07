#pragma once
#include "Texture.hpp"
#include <memory>
#include <assimp/material.h>
#include <string>
#include <assimp/texture.h>
class Texture2D : public Texture
{
public:
	static std::shared_ptr<Texture2D> createTexture2D(const std::string& file, aiTextureType type);
	static std::shared_ptr<Texture2D> createTexture2D(const aiTexture* aiTex, aiTextureType type);
};

