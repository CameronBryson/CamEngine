#pragma once
#include <memory>
#include <string>


class Texture
{
public:
	virtual ~Texture() = default;
	virtual void bind() const = 0;
	virtual void unbind() = 0;
	virtual void deleteTexture() const = 0;
	static std::shared_ptr<Texture> createTexture(const std::string& file);
};