#include "pch.hpp"
#include "Font.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include "Shader.hpp"
#include <iostream>

Font::Font(const std::string& fontPath, float fontSize)
	: mVAO(0), mVBO(0), mFtLib(nullptr), mFace(nullptr)
{
	initializeFreeType();
	loadFontFace(fontPath, fontSize);
	loadCharacters();
	setupBuffers();
}

Font::~Font()
{
	// Clean up textures
	for (auto& pair : mCharacterMap)
	{
		glDeleteTextures(1, &pair.second.TextureID);
	}

	// Clean up VAO/VBO
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);

	// Clean up FreeType resources
	if (mFace)
		FT_Done_Face(mFace);
	if (mFtLib)
		FT_Done_FreeType(mFtLib);
}

void Font::initializeFreeType()
{
	// Initialize FreeType
	if (FT_Init_FreeType(&mFtLib))
	{
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		mFtLib = nullptr;
	}
}

void Font::loadFontFace(const std::string& fontPath, float fontSize)
{
	if (!mFtLib)
		return;

	// Load font face
	if (FT_New_Face(mFtLib, engine_util::buildPath(fontPath).c_str(), 0, &mFace))
	{
		std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
		mFace = nullptr;
		return;
	}

	// Set font size
	FT_Set_Pixel_Sizes(mFace, 0, static_cast<FT_UInt>(fontSize));
}

void Font::loadCharacters()
{
	if (!mFace)
		return;

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(mFace, c, FT_LOAD_RENDER))
		{
			std::cerr << "ERROR::FREETYPE: Failed to load Glyph '" << c << "'" << std::endl;
			continue;
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			mFace->glyph->bitmap.width,
			mFace->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			mFace->glyph->bitmap.buffer
		);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Store character for later use
		Character character = {
			texture,
			glm::ivec2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
			glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
			static_cast<GLuint>(mFace->glyph->advance.x)
		};
		mCharacterMap.insert(std::pair<char, Character>(c, character));
	}
}

void Font::setupBuffers()
{
	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Font::renderText(Shader& shader, const std::string& text, float x, float y, float scale, const glm::vec3& color)
{
	// Activate corresponding render state
	shader.use();
	shader.setVec3("textColor", color);
	shader.setInt("text", 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(mVAO);

	// Iterate through all characters
	for (const char& c : text)
	{
		Character ch = mCharacterMap[c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		// Update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Advance cursor for next glyph
		x += (ch.Advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_BLEND);
}
