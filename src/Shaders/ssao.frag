#version 460 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;   // Position from G-buffer
uniform sampler2D gNormal;     // Normal from G-buffer
uniform sampler2D texNoise;    // Random rotation vectors

uniform vec3 samples[64];      // Sample kernel (SSAO_KERNEL_SIZE)
uniform mat4 projection;
uniform mat4 view;

uniform float radius;          // Maps to mSSAORadius
uniform float bias;           // Maps to mSSAOBias
uniform float power;          // Maps to mSSAOPower

// Screen-space noise tiling (SSAO_NOISE_SIZE is 4x4)
const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); 

void main()
{
    // Get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
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
        
        // Project sample position
        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;             
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z;
        
        // Range and occlusion test
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = pow(occlusion, power);
}

