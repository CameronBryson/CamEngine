#include "TextRenderer.hpp"

#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_clip_space.hpp"

void TextRenderer::init()
{
	
}

void TextRenderer::loadFont(const std::string& fontPath, int fontSize)
{
}

void TextRenderer::renderText(ShaderProgram& textShader, const std::string& text, const glm::vec2& position,
	float scale, const glm::vec3& color)
{
	textShader.use();
	textShader.setVec3("textColor", color);

	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

}

void TextRenderer::shutdown()
{
}
