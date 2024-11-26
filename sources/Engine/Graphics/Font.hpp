#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <unordered_map>
#include <glm/vec3.hpp>
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Util/platform.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character
{
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Offset to advance to next glyph
};

class Font
{
public:
    Font(const std::string& fontPath, float fontSize);
    ~Font();

    void renderText(Shader& shader, const std::string& text, float x, float y, float scale, const glm::vec3& color);

    // Disable copy semantics
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

private:
    void initializeFreeType();
    void loadFontFace(const std::string& fontPath, float fontSize);
    void loadCharacters();
    void setupBuffers();

    FT_Library mFtLib;
    FT_Face mFace;
    std::unordered_map<char, Character> mCharacterMap;
    GLuint mVAO, mVBO;
};