#version 460 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraUBO
{
    mat4 uView;
    mat4 uProjection;
    vec4 uCameraPos;
};

uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = uProjection * uView * worldPos;
}
