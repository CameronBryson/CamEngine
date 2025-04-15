#pragma once

#include <string>
#include <unordered_map>
#include <glm/fwd.hpp>
#include "Engine/Util/Logging.hpp"
#include "platform.hpp"
#include <string_view>
/**
 * @brief Represents an OpenGL shader program
 * 
 * Handles the compilation, linking and usage of OpenGL shader programs.
 * Supports vertex, fragment, geometry, and compute shaders.
 * Provides utility functions to set uniform values.
 */
class Shader
{
public:
    /**
     * @brief Creates a shader program from vertex and fragment shader files
     * 
     * @param vertexPath Path to the vertex shader file
     * @param fragmentPath Path to the fragment shader file
     */
    Shader(std::string_view vertexPath, std::string_view fragmentPath);
    
    /**
     * @brief Creates a shader program from vertex, fragment and geometry shader files
     * 
     * @param vertexPath Path to the vertex shader file
     * @param fragmentPath Path to the fragment shader file
     * @param geometryPath Path to the geometry shader file
     */
    Shader(std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath);
    
    /**
     * @brief Destructor cleans up OpenGL resources
     */
    ~Shader();
    
    // Delete copy constructor and assignment operator to prevent double-deletion of OpenGL resources
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // Allow move semantics for efficient container usage
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    /**
     * @brief Activates the shader program for rendering
     */
    void use() const;
    
    
    /**
     * @brief Set a boolean uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Boolean value to set
     */
    void setBool(const std::string& name, bool value) const;
    
    /**
     * @brief Set an integer uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Integer value to set
     */
    void setInt(const std::string& name, int value) const;
    
    /**
     * @brief Set a float uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Float value to set
     */
    void setFloat(const std::string& name, float value) const;
    
    /**
     * @brief Set a 2D vector uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Vector to set
     */
    void setVec2(const std::string& name, const glm::vec2& value) const;
    
    /**
     * @brief Set a 3D vector uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Vector to set
     */
    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    /**
     * @brief Set a 4D vector uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Vector to set
     */
    void setVec4(const std::string& name, const glm::vec4& value) const;
    
    /**
     * @brief Set a 4x4 matrix uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Matrix to set
     */
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
    /**
     * @brief Set a 3x3 matrix uniform value
     * 
     * @param name Name of the uniform in the shader
     * @param value Matrix to set
     */
    void setMat3(const std::string& name, const glm::mat3& value) const;

    /**
     * @brief Gets the OpenGL shader program ID
     * 
     * @return Unsigned int representing the shader program ID
     */
    unsigned int getID() const { return mShaderID; }
    
    
	bool isCompiled() const { return mShaderID != 0; }
    /**
     * @brief Force delete the shader program
     */
    void deleteShader();

private:
    /**
     * @brief Compiles a shader from source code
     * 
     * @param type Shader type (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.)
     * @param source Shader source code
     * @param shaderPath Path to the shader file (for error reporting)
     * @return Compiled shader ID
     */
    unsigned int compileShader(unsigned int type, std::string_view source, std::string_view shaderPath);
    
    /**
     * @brief Checks for shader compilation or program linking errors
     * 
     * @param shader Shader or program ID to check
     * @param type Type string for error reporting ("VERTEX", "FRAGMENT", "PROGRAM", etc.)
     * @param filePath Path to the shader file for error reporting
     * @throws GraphicsException if compilation/linking fails
     */
    void checkCompileError(unsigned int shader, const std::string& type, const std::string& filePath = "");
    
    /**
     * @brief Loads and processes a shader file
     * 
     * @param filePath Path to the shader file
     * @return String containing shader source code
     */
    std::string loadShaderFile(std::string_view filePath);
    
    /**
     * @brief Gets cached location of a uniform
     * 
     * @param name Uniform name
     * @return Uniform location
     */
    int getUniformLocation(const std::string& name) const;

private:
    GLuint mShaderID = 0;
    mutable std::unordered_map<std::string, int> mUniformLocationCache;
};
