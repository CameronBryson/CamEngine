#version 460 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraBlock {
    vec4 cameraPos;              
    mat4 view;                  
    mat4 projection;
    mat4 viewProjection;
    mat4 inverseView;
    mat4 inverseProjection;
    mat4 inverseViewProjection;
    mat4 previousView;           
    mat4 previousProjection;
    mat4 previousViewProjection;
};

uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = viewProjection * worldPos;
}
