#pragma once

#include <string>
#include <memory>
#include "Engine/Util/platform.hpp"
#include <assimp/texture.h>
#include <glm/glm.hpp>

class Shader;

/**
 * Unified Texture class that handles both 2D textures and cubemaps
 */
class Texture
{
public:
    // Texture formats for internal use
    enum class Format {
        R,          // Single channel (red)
        RG,         // Two channels (red, green)
        RGB,        // Three channels
        RGBA,       // Four channels
        RGB16F,     // HDR three channels
        RGBA16F,    // HDR four channels
        R32F,       // 32-bit float single channel
        Depth,      // Depth component
        DepthStencil // Combined depth-stencil
    };
    
    // Texture types
    enum class Type {
        TEXTURE_2D,
        CUBEMAP
    };
    
    // Filter modes for minification and magnification
    enum class FilterMode {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear
    };
    
    // Wrap modes for texture coordinates
    enum class WrapMode {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };
    
    // Compare modes for shadow sampling
    enum class CompareMode {
        None,
        CompareRefToTexture
    };
    
    // Compare functions for shadow sampling
    enum class CompareFunc {
        LessEqual,
        GreaterEqual,
        Less,
        Greater,
        Equal,
        NotEqual,
        Always,
        Never
    };

public:
    // Constructors for different texture sources
    
    // Create an empty 2D texture with specified dimensions and format
    Texture(int width, int height, Format format);
    
    // Load a 2D texture from a file
    Texture(const std::string& filePath);
    
    // Create a cubemap texture from an HDR equirectangular map
    Texture(const std::string& equirectangularMapPath, const std::shared_ptr<Shader>& equirectShader);
    
    // Create a texture from an Assimp texture object
    Texture(const aiTexture* aiTex);
    
    // Create a texture from an existing OpenGL texture ID
    Texture(GLuint textureID, int width, int height, Type type = Type::TEXTURE_2D);
    
    // Destructor
    ~Texture();
    
    // Copy and move semantics
    Texture(const Texture&) = delete;             // No copy constructor
    Texture& operator=(const Texture&) = delete;  // No copy assignment
    Texture(Texture&& other) noexcept;            // Move constructor
    Texture& operator=(Texture&& other) noexcept; // Move assignment

    // Core functionality
    void bind(unsigned int slot = 0);
    void unbind(unsigned int slot = 0);
    
    // Getters
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    unsigned int getTextureID() const { return mTextureID; }
    Type getType() const { return mType; }
    bool isShadowSampler() const { return mIsShadowSampler; }
    bool isNormalMap() const { return mIsNormalMap; }
    
    // Texture sampler configurations
    void setShadowSamplerParameters();
    void setNormalSamplerParameters();
    
    // Set texture filtering modes
    void setFilterMode(FilterMode minFilter, FilterMode magFilter);
    
    // Set texture wrapping modes
    void setWrapMode(WrapMode wrapS, WrapMode wrapT, WrapMode wrapR = WrapMode::Repeat);
    
    // Set compare mode for shadow sampling
    void setCompareMode(CompareMode mode, CompareFunc func = CompareFunc::LessEqual);
    
    // Set border color for WrapMode::ClampToBorder
    void setBorderColor(const glm::vec4& color);
    
    // Generate mipmaps for the texture
    void generateMipmaps();
    
    // Static helper methods
    static GLuint createEmptyTexture2D(int width, int height, Format format);
    static GLuint createEmptyCubemap(int size, Format format);

private:
    // Convert enum values to OpenGL constants
    static GLenum toGLInternalFormat(Format format);
    static GLenum toGLFormat(Format format);
    static GLenum toGLType(Format format);
    static GLenum toGLFilterMode(FilterMode mode);
    static GLenum toGLWrapMode(WrapMode mode);
    static GLenum toGLCompareFunc(CompareFunc func);
    static GLenum toGLTextureTarget(Type type);

private:
    GLuint mTextureID = 0;
    int mWidth = 0;
    int mHeight = 0;
    Type mType = Type::TEXTURE_2D;
    bool mIsShadowSampler = false;
    bool mIsNormalMap = false;
    Format mFormat = Format::RGBA;
};