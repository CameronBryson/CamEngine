#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D gNormalMetallic; // RGB: Normal, A: Metallic
uniform sampler2D gRoughEmissive;  // R: Roughness, GBA: Emissive
uniform float threshold;
uniform float softThreshold;

// Convert RGB to luminance using perceptual weights
float getLuminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main()
{
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    float roughness = texture(gRoughEmissive, TexCoords).r;
    vec3 normal = texture(gNormalMetallic, TexCoords).rgb;
    
    // Calculate brightness using luminance
    float brightness = getLuminance(hdrColor);
    
    // Ensure we have valid normal data before calculations
    if (length(normal) < 0.1) {
        // Default to simple threshold for skybox or areas without geometry
        float softness = smoothstep(threshold, threshold + softThreshold, brightness);
        FragColor = vec4(hdrColor * softness, 1.0);
        return;
    }
    
    // Normal is in view space, so (0,0,1) is pointing toward the camera
    normal = normalize(normal);
    
    // Modify bloom threshold based on surface roughness
    // Rough surfaces scatter more light at grazing angles
    float adjustedThreshold = threshold * (1.0 - roughness * 0.5);
    
    // Calculate light intensity falloff based on viewing angle (Fresnel-like effect)
    float NdotV = max(dot(normal, vec3(0.0, 0.0, 1.0)), 0.0);
    float fresnelFactor = pow(1.0 - NdotV, 3.0) * roughness;
    
    // Apply soft threshold with roughness-based adjustment
    float softness = clamp((brightness - adjustedThreshold) / max(softThreshold, 0.01), 0.0, 1.0);
    softness = softness * (1.0 + fresnelFactor);
    
    // Use physically-based energy conservation with safeguards
    vec3 brightColor = hdrColor * softness * (brightness / max(brightness, 0.001));
    
    // Add a minimum value to prevent complete blackness due to precision errors
    brightColor = max(brightColor, vec3(0.0001));
    
    FragColor = vec4(brightColor, 1.0);
}
