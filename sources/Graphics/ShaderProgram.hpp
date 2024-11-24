#pragma once
#include <glm/fwd.hpp>
#include <string>

#include <Engine/EngineUtil.hpp>


class ShaderProgram
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    explicit ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

    // activate the shader
    // ------------------------------------------------------------------------
    void use() const;

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const;

    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const;

    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const;

    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const;


    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const;


    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const;


    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};
