#version 460 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gNormalMetallic;  // RGB: World space normal, A: Metallic 
uniform sampler2D gDepth;           // Depth from G-buffer
uniform sampler2D texNoise;         // Random rotation vectors

uniform vec3 samples[64];           // Sample kernel (SSAO_KERNEL_SIZE)
uniform mat4 projection;
uniform mat4 view;
uniform mat4 inverseProjection;     // Now used directly

uniform float radius;               // Maps to mSSAORadius
uniform float bias;                 // Maps to mSSAOBias
uniform float power;                // Maps to mSSAOPower
uniform vec2 resolution;

// Screen-space noise tiling (SSAO_NOISE_SIZE is 4x4)
const vec2 noiseScale = vec2(resolution.x/4.0, resolution.y/4.0); 

// Optimized function using inverseProjection uniform directly
vec3 reconstructViewPosition(float depth, vec2 texCoords) {
    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePosition = inverseProjection * clipSpacePosition;
    return viewSpacePosition.xyz / viewSpacePosition.w;
}

void main()
{
    // Get depth
    float depth = texture(gDepth, TexCoords).r;
    
    // Reconstruct position from depth in view space
    vec3 fragPos = reconstructViewPosition(depth, TexCoords);
    
    // Get world space normal from G-buffer and transform to view space
    vec3 normal = texture(gNormalMetallic, TexCoords).rgb * 2.0 - 1.0;
    normal = normalize(mat3(view) * normal); // Transform normal to view space
    
    // Get random vector and transform to view space
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz * 2.0 - 1.0;
    randomVec = normalize(mat3(view) * randomVec);
    
    // Create TBN matrix (already in view space now)
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // Calculate occlusion
    float occlusion = 0.0;
    for(int i = 0; i < 64; ++i) // SSAO_KERNEL_SIZE
    {
        // Get sample position in view space
        vec3 samplePos = TBN * samples[i]; 
        samplePos = fragPos + samplePos * radius;
        
        // Project sample position
        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;             
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Get sample depth
        float sampleDepth = texture(gDepth, offset.xy).r;
        vec3 sampleViewPos = reconstructViewPosition(sampleDepth, offset.xy);
        
        // Range check in view space
        float rangeCheck = smoothstep(0.0, 1.0, radius / length(fragPos - sampleViewPos));
        
        // Occlusion test in view space (note: view space Z is negative as it points into the screen)
        occlusion += (sampleViewPos.z >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = pow(occlusion, power);
}
