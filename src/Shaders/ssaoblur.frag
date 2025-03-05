#version 460 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform sampler2D gDepth;
uniform sampler2D gNormalMetallic;

uniform float blurRadius;
uniform float depthThreshold;
uniform float normalThreshold;

// Pre-computed Gaussian weights
const float gaussianWeights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec3 GetNormal(vec2 coord) {
    return normalize(texture(gNormalMetallic, coord).rgb * 2.0 - 1.0);
}

float GetDepth(vec2 coord) {
    return texture(gDepth, coord).r;
}

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    
    // Fetch center values only once
    float centerDepth = GetDepth(TexCoords);
    vec3 centerNormal = GetNormal(TexCoords);
    float centerAO = texture(ssaoInput, TexCoords).r;
    
    // Initialize with center sample
    float sum = centerAO;
    float weightSum = 1.0;
    
    // Two-pass separable Gaussian blur (horizontal + vertical)
    // This reduces texture fetches from 81 to 18 (9 horizontal + 9 vertical)
    
    // First pass: horizontal blur
    for(int i = 1; i <= 4; ++i) {
        // Calculate offset for this sample
        float offset = float(i) * texelSize.x * blurRadius;
        
        // Sample to the right
        vec2 sampleCoordRight = TexCoords + vec2(offset, 0.0);
        if (sampleCoordRight.x <= 1.0) {
            // Test if we should fetch depth and normal
            float sampleAO = texture(ssaoInput, sampleCoordRight).r;
            float approxWeight = gaussianWeights[i];
            
            // Only fetch depth and normal if AO value is significantly different
            // This is an early optimization to avoid unneeded texture fetches
            if (abs(sampleAO - centerAO) * approxWeight > 0.01) {
                float sampleDepth = GetDepth(sampleCoordRight);
                vec3 sampleNormal = GetNormal(sampleCoordRight);
                
                // Calculate weights based on depth and normal similarity
                float depthDiff = abs(centerDepth - sampleDepth);
                float normalDiff = 1.0 - dot(centerNormal, sampleNormal);
                
                // Early rejection for samples that will have near-zero weight
                if (depthDiff < depthThreshold * 3.0 && normalDiff < normalThreshold) {
                    float depthWeight = exp(-depthDiff * depthThreshold);
                    float normalWeight = step(normalDiff, normalThreshold);
                    float weight = depthWeight * normalWeight * gaussianWeights[i];
                    
                    sum += sampleAO * weight;
                    weightSum += weight;
                }
            }
        }
        
        // Sample to the left
        vec2 sampleCoordLeft = TexCoords - vec2(offset, 0.0);
        if (sampleCoordLeft.x >= 0.0) {
            float sampleAO = texture(ssaoInput, sampleCoordLeft).r;
            float approxWeight = gaussianWeights[i];
            
            if (abs(sampleAO - centerAO) * approxWeight > 0.01) {
                float sampleDepth = GetDepth(sampleCoordLeft);
                vec3 sampleNormal = GetNormal(sampleCoordLeft);
                
                float depthDiff = abs(centerDepth - sampleDepth);
                float normalDiff = 1.0 - dot(centerNormal, sampleNormal);
                
                if (depthDiff < depthThreshold * 3.0 && normalDiff < normalThreshold) {
                    float depthWeight = exp(-depthDiff * depthThreshold);
                    float normalWeight = step(normalDiff, normalThreshold);
                    float weight = depthWeight * normalWeight * gaussianWeights[i];
                    
                    sum += sampleAO * weight;
                    weightSum += weight;
                }
            }
        }
    }
    
    // Intermediate result after horizontal pass
    float horizontalBlur = sum / weightSum;
    
    // Reset for vertical pass
    sum = horizontalBlur;
    weightSum = 1.0;
    
    // Second pass: vertical blur (using result from horizontal pass)
    for(int i = 1; i <= 4; ++i) {
        float offset = float(i) * texelSize.y * blurRadius;
        
        // Sample above
        vec2 sampleCoordUp = TexCoords + vec2(0.0, offset);
        if (sampleCoordUp.y <= 1.0) {
            float sampleAO = texture(ssaoInput, sampleCoordUp).r;
            float approxWeight = gaussianWeights[i];
            
            if (abs(sampleAO - centerAO) * approxWeight > 0.01) {
                float sampleDepth = GetDepth(sampleCoordUp);
                vec3 sampleNormal = GetNormal(sampleCoordUp);
                
                float depthDiff = abs(centerDepth - sampleDepth);
                float normalDiff = 1.0 - dot(centerNormal, sampleNormal);
                
                if (depthDiff < depthThreshold * 3.0 && normalDiff < normalThreshold) {
                    float depthWeight = exp(-depthDiff * depthThreshold);
                    float normalWeight = step(normalDiff, normalThreshold);
                    float weight = depthWeight * normalWeight * gaussianWeights[i];
                    
                    sum += sampleAO * weight;
                    weightSum += weight;
                }
            }
        }
        
        // Sample below
        vec2 sampleCoordDown = TexCoords - vec2(0.0, offset);
        if (sampleCoordDown.y >= 0.0) {
            float sampleAO = texture(ssaoInput, sampleCoordDown).r;
            float approxWeight = gaussianWeights[i];
            
            if (abs(sampleAO - centerAO) * approxWeight > 0.01) {
                float sampleDepth = GetDepth(sampleCoordDown);
                vec3 sampleNormal = GetNormal(sampleCoordDown);
                
                float depthDiff = abs(centerDepth - sampleDepth);
                float normalDiff = 1.0 - dot(centerNormal, sampleNormal);
                
                if (depthDiff < depthThreshold * 3.0 && normalDiff < normalThreshold) {
                    float depthWeight = exp(-depthDiff * depthThreshold);
                    float normalWeight = step(normalDiff, normalThreshold);
                    float weight = depthWeight * normalWeight * gaussianWeights[i];
                    
                    sum += sampleAO * weight;
                    weightSum += weight;
                }
            }
        }
    }
    
    // Final blurred result
    FragColor = sum / weightSum;
}
