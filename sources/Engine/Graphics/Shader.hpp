#pragma once
#include <memory>
#include <glm/fwd.hpp>
#include <string>

class Shader
{
public:

	virtual ~Shader() = default;

    virtual void use() const = 0;

    virtual void setBool(const std::string& name, bool value) const = 0;

    virtual void setInt(const std::string& name, int value) const = 0;

	virtual void setFloat(const std::string& name, float value) const = 0;

	virtual void setVec2(const std::string& name, const glm::vec2& value) const = 0;


    virtual void setVec3(const std::string& name, const glm::vec3& value) const = 0;


    virtual void setVec4(const std::string& name, const glm::vec4& value) const = 0;


    virtual void setMat4(const std::string& name, const glm::mat4& mat) const = 0;


	static std::shared_ptr<Shader> createShader(const std::string& vertexPath, const std::string& fragmentPath);
};
