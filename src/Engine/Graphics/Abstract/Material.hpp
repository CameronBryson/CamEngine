#pragma once
class Shader;
class Material
{
public:
	virtual ~Material() = default;
	virtual void bind(const Shader& shader) = 0;
	virtual void unbind() = 0;
};
