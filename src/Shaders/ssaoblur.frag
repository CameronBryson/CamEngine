#version 460 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform sampler2D gDepth;
uniform sampler2D gNormalMetallic;

uniform float blurRadius;
uniform float depthThreshold;
uniform float normalThreshold;

const int kernelSize = 9;
const float gaussianWeights[9] = float[](0.106, 0.140, 0.165, 0.175, 0.165, 0.140, 0.106, 0.067, 0.028);

vec3 GetNormal(vec2 coord) {
    return normalize(texture(gNormalMetallic, coord).rgb * 2.0 - 1.0);
}

float GetDepth(vec2 coord) {
    return texture(gDepth, coord).r;
}

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float centerDepth = GetDepth(TexCoords);
    vec3 centerNormal = GetNormal(TexCoords);
    
    float sum = 0.0;
    float weightSum = 0.0;
    
    for(int x = -4; x <= 4; ++x)
    {
        for(int y = -4; y <= 4; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize * blurRadius;
            vec2 sampleCoord = TexCoords + offset;
            
            float sampleDepth = GetDepth(sampleCoord);
            vec3 sampleNormal = GetNormal(sampleCoord);
            
            // Depth similarity
            float depthDiff = abs(centerDepth - sampleDepth);
            float depthWeight = exp(-depthDiff * depthThreshold);
            
            // Normal similarity
            float normalDiff = 1.0 - dot(centerNormal, sampleNormal);
            float normalWeight = step(normalDiff, normalThreshold);
            
            // Combine weights
            float weight = depthWeight * normalWeight * gaussianWeights[abs(x)] * gaussianWeights[abs(y)];
            sum += texture(ssaoInput, sampleCoord).r * weight;
            weightSum += weight;
        }
    }
    
    FragColor = sum / weightSum;
}