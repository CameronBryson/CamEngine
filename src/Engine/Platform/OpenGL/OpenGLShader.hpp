#pragma once
#include <glm/fwd.hpp>
#include <string>
#include "Shader.hpp"

class OpenGLShader : public Shader
{
public:
    explicit OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath);
	OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);

    ~OpenGLShader();

    void use() const override;

    void setBool(const std::string& name, bool value) const override;

    void setInt(const std::string& name, int value) const override;

    void setFloat(const std::string& name, float value) const override;

    void setVec2(const std::string& name, const glm::vec2& value) const override;


    void setVec3(const std::string& name, const glm::vec3& value) const override;


    void setVec4(const std::string& name, const glm::vec4& value) const override;


    void setMat4(const std::string& name, const glm::mat4& value) const override;

    void deleteShader() override;

	void checkCompileError(unsigned shader, const std::string& type);
private:
    unsigned int shaderID;
};
