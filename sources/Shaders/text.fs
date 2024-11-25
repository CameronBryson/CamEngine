#version 330 core
in vec2 TexCoords;
out ve4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	float alpha = texture(text, TexCoords).r;
	FragColor = vec4(textColor, alpha);
}