#version 460 core

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
    mat4 uPrevView;
    mat4 uPrevProjection;
};

uniform mat4 model; // Per-object transform still remains a normal uniform

out vec2 TexCoord;
out vec3 FragPos;
out mat3 TBN;
out vec4 ClipPos;
out vec4 PrevClipPos;

void main()
{
    // Compute world-space position of this vertex
    TexCoord = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Calculate TBN matrix for normal mapping
    mat3 normalMatrix = transpose(inverse(mat3(model))); 
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 B = normalize(normalMatrix * aBitangent);
    
    // Re-orthogonalize T with respect to N (Gram-Schmidt process)
    T = normalize(T - dot(T, N) * N);
    
    // Standard TBN for transforming from tangent to world space
    TBN = mat3(T, B, N);
    
    // Calculate clip positions for current and previous frames
    ClipPos = uProjection * uView * vec4(FragPos, 1.0);
    PrevClipPos = uPrevProjection * uPrevView * vec4(FragPos, 1.0);
    gl_Position = ClipPos;
}



