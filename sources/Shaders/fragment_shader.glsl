#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    vec3 ambient;
    float shininess;
};

uniform Material material;

void main()
{
    // Sample the diffuse texture
    vec3 color = texture(material.diffuse, TexCoord).rgb;
    FragColor = vec4(color, 1.0);
}