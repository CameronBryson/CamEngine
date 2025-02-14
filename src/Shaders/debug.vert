#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec2 TexCoords;
out vec3 Normal;
out vec4 FragPosLightSpace;

void main()
{
    TexCoords = aTexCoords;
    Normal = aNormal;
    
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPosLightSpace = lightSpaceMatrix * worldPos;
    gl_Position = FragPosLightSpace;
}
