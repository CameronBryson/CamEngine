#version 460 core
precision highp float;

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gNormalMetallic;  // RGB: World space normal, A: Metallic
uniform sampler2D gVelocityReflective; // RG: Screen-space Velocity, B Reflective , A Unused
uniform sampler2D gDepth; // Depth from G-buffer
uniform sampler2D gSceneColor; // Scene color from G-buffer

layout(std140, binding = 0) uniform CameraBlock {
    mat4 view;                  
    mat4 projection;            
    vec4 cameraPos;              
    mat4 previousView;           
    mat4 previousProjection;     
};

uniform float maxDistance;
uniform float resolution;
uniform float thickness;
uniform int steps;
vec3 reconstructViewPosition(float depth, vec2 texCoords) {
    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePosition = inverse(projection) * clipSpacePosition;
    return viewSpacePosition.xyz / viewSpacePosition.w;
}
// Helper function to convert world position to screen coordinates
vec2 worldToScreen(vec3 worldPos) {
    vec4 clipPos = projection * view * vec4(worldPos, 1.0);
    vec2 ndc = clipPos.xy / clipPos.w;
    return ndc * 0.5 + 0.5;
}
void main()
{
//    FragColor = vec4(texture(gNormalMetallic, TexCoord).rgb, 1.0);
//    return;
    vec3 color = texture(gSceneColor, TexCoord).rgb;
    float reflectivity = texture(gVelocityReflective, TexCoord).b;
    if (reflectivity < 0.05)
    {
        FragColor = vec4(color,1);
        return;
    }
    // Unpack the normal from [0,1] to [-1,1] space
    vec3 normal = normalize(texture(gNormalMetallic, TexCoord).rgb * 2.0 - 1.0);
    float depth = texture(gDepth, TexCoord).r;

    // Early rejection for sky or invalid depth
    if (depth >= 0.9999) {
        FragColor = vec4(color, 1.0);
        return;
    }

    vec3 worldPos = reconstructViewPosition(depth, TexCoord);
    vec3 viewDir = normalize(worldPos - cameraPos.xyz);
    vec3 reflectionDir = reflect(viewDir, normal);
//    if(dot(reflectionDir, normal) < 0.0)
//    {
//        FragColor = vec4(1.0,0,0, 1.0);
//        return;
//    }
    FragColor = vec4(color, 1.0);


}