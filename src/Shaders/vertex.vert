#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

// Remove old uniform mat4 view, uniform mat4 projection
// Instead, define a uniform block for camera data:
layout(std140, binding = 0) uniform CameraUBO
{
    mat4 uView;
    mat4 uProjection;
    vec4 uCameraPos; // .xyz used if you need camera pos here, .w is padding
};

uniform mat4 model; // Per-object transform still remains a normal uniform

out vec2 TexCoord;
out vec3 FragPos;
out mat3 TBN;

void main()
{
    // Compute world-space position of this vertex
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Just pass the UV coords through
    TexCoord = aTexCoords;

    // Construct TBN from model transform:
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);
    TBN = mat3(T, B, N);

    // Final position uses the camera UBO data
    gl_Position = uProjection * uView * model * vec4(aPos, 1.0);
}
