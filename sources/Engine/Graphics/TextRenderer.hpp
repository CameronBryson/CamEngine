#pragma once
#include <unordered_map>
#include <string>
#include <glm/vec2.hpp>
#include "ShaderProgram.hpp"
#include "Texture.hpp"

struct Character
{
	Texture texture;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};
class TextRenderer
{
public:
	void init();
	void loadFont(const std::string& fontPath, int fontSize);
	void renderText(ShaderProgram& textShader, const std::string& text, const glm::vec2& position, float scale, const glm::vec3& color);
	void shutdown();
private:
	std::unordered_map<char, Character> mCharacterMap;

};