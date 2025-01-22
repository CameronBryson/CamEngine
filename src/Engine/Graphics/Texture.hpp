#pragma once
#include <memory>
#include <string>

#include "assimp/material.h"
class Texture
{
public:
	virtual ~Texture() = default;
	virtual void bind(unsigned int slot) const = 0;
	virtual void unbind(unsigned int slot) = 0;
	virtual void deleteTexture() const = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual unsigned char* getData() const = 0;
	static std::shared_ptr<Texture> createTexture(const std::string& file, aiTextureType type);
};