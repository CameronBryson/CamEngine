#version 460 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gNormalMetallic;  // RGB: World space normal, A: Metallic 
uniform sampler2D gDepth;           // Depth from G-buffer
uniform sampler2D texNoise;         // Random rotation vectors

uniform vec3 samples[64];           // Sample kernel (SSAO_KERNEL_SIZE)
uniform mat4 projection;
uniform mat4 view;

uniform float radius;               // Maps to mSSAORadius
uniform float bias;                 // Maps to mSSAOBias
uniform float power;                // Maps to mSSAOPower

// Screen-space noise tiling (SSAO_NOISE_SIZE is 4x4)
const vec2 noiseScale = vec2(2560.0/4.0, 1440.0/4.0); 

// Function to reconstruct position from depth
vec3 reconstructWorldPosition(float depth, vec2 texCoords) {
    // Convert to NDC space
    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    
    // Convert to view space
    vec4 viewSpacePosition = inverse(projection) * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    
    // Convert to world space
    vec4 worldSpacePosition = inverse(view) * viewSpacePosition;
    
    return worldSpacePosition.xyz;
}

void main()
{
    // Get depth
    float depth = texture(gDepth, TexCoords).r;
    
    // Reconstruct position from depth
    vec3 fragPos = reconstructWorldPosition(depth, TexCoords);
    
    // Get normal from G-buffer (stored as [0,1], convert back to [-1,1])
    vec3 normal = texture(gNormalMetallic, TexCoords).rgb * 2.0 - 1.0;
    normal = normalize(normal);
    
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    
    // Create TBN matrix
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // Calculate occlusion
    float occlusion = 0.0;
    for(int i = 0; i < 64; ++i) // SSAO_KERNEL_SIZE
    {
        // Get sample position
        vec3 samplePos = TBN * samples[i]; 
        samplePos = fragPos + samplePos * radius;
        
        // Project sample position to get sample coordinates
        vec4 offset = projection * view * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;             
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Get sample depth
        float sampleDepth = texture(gDepth, offset.xy).r;
        vec3 sampleWorldPos = reconstructWorldPosition(sampleDepth, offset.xy);
        
        // Range and occlusion test
        float rangeCheck = smoothstep(0.0, 1.0, radius / length(fragPos - sampleWorldPos));
        occlusion += (sampleWorldPos.z >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = pow(occlusion, power);
}
