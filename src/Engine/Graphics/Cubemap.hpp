#pragma once
#include <memory>
#include <string>
#include <vector>
class Shader;
class Cubemap
{
public:
	virtual ~Cubemap() = default;
	virtual void draw(const Shader& shader) const = 0;
	virtual int getID() const = 0;
	static std::shared_ptr<Cubemap> createCubemap(const std::vector<std::string> facePaths);

};

