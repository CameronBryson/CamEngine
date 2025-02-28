#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D currentFrame;
uniform sampler2D previousFrame; 
uniform sampler2D velocityMap;
uniform float blendFactor;
uniform vec2 resolution;
uniform bool showEdges;
uniform float edgeThreshold;
uniform float edgeResponsiveness;
uniform float minBlend;

vec2 getVelocity(vec2 uv) {
    return texture(velocityMap, uv).rg;
}

vec3 sampleCurrentFrame(vec2 uv) {
    return texture(currentFrame, uv).rgb;
}

vec3 samplePreviousFrame(vec2 uv) {
    return texture(previousFrame, uv).rgb;
}

// Helper function to calculate luma (brightness) from RGB
float rgb2luma(vec3 rgb) {
    return dot(rgb, vec3(0.299, 0.587, 0.114));
}

// Function to calculate neighborhood bounds for history clamping
void calculateNeighborhoodMinMax(vec2 uv, out vec3 minColor, out vec3 maxColor) {
    // Sample all 9 pixels in a 3x3 grid
    vec3 c00 = sampleCurrentFrame(uv + vec2(-1, -1) / resolution);
    vec3 c10 = sampleCurrentFrame(uv + vec2(0, -1) / resolution);
    vec3 c20 = sampleCurrentFrame(uv + vec2(1, -1) / resolution);
    vec3 c01 = sampleCurrentFrame(uv + vec2(-1, 0) / resolution);
    vec3 c11 = sampleCurrentFrame(uv); // Center pixel
    vec3 c21 = sampleCurrentFrame(uv + vec2(1, 0) / resolution);
    vec3 c02 = sampleCurrentFrame(uv + vec2(-1, 1) / resolution);
    vec3 c12 = sampleCurrentFrame(uv + vec2(0, 1) / resolution);
    vec3 c22 = sampleCurrentFrame(uv + vec2(1, 1) / resolution);
    
    // Calculate min/max bounds across all samples
    minColor = min(c11, min(c00, min(c10, min(c20, min(c01, min(c21, min(c02, min(c12, c22))))))));
    maxColor = max(c11, max(c00, max(c10, max(c20, max(c01, max(c21, max(c02, max(c12, c22))))))));
    
    // Calculate average color
    vec3 avgColor = (c00 + c10 + c20 + c01 + c11 + c21 + c02 + c12 + c22) / 9.0;
    
    // Expand the bounding box for bright highlights to reduce flickering
    vec3 variance = (maxColor - minColor);
    float luminance = rgb2luma(avgColor);
    
    // For bright areas, expand the range more
    float expansion = 1.0 + 2.0 * luminance;
    minColor -= variance * 0.1 * expansion;
    maxColor += variance * 0.1 * expansion;
}





// Enhanced edge detection using sobel operator and variance
float calculateEdgeStrength(vec2 uv) {
    // Sample 3x3 neighborhood
    vec3 c00 = sampleCurrentFrame(uv + vec2(-1, -1) / resolution);
    vec3 c10 = sampleCurrentFrame(uv + vec2(0, -1) / resolution);
    vec3 c20 = sampleCurrentFrame(uv + vec2(1, -1) / resolution);
    vec3 c01 = sampleCurrentFrame(uv + vec2(-1, 0) / resolution);
    vec3 c11 = sampleCurrentFrame(uv); // Center pixel
    vec3 c21 = sampleCurrentFrame(uv + vec2(1, 0) / resolution);
    vec3 c02 = sampleCurrentFrame(uv + vec2(-1, 1) / resolution);
    vec3 c12 = sampleCurrentFrame(uv + vec2(0, 1) / resolution);
    vec3 c22 = sampleCurrentFrame(uv + vec2(1, 1) / resolution);
    
    // Convert to luma for edge detection
    float l00 = rgb2luma(c00);
    float l10 = rgb2luma(c10);
    float l20 = rgb2luma(c20);
    float l01 = rgb2luma(c01);
    float l11 = rgb2luma(c11);
    float l21 = rgb2luma(c21);
    float l02 = rgb2luma(c02);
    float l12 = rgb2luma(c12);
    float l22 = rgb2luma(c22);

    float avgLuminance = (l00 + l10 + l20 + l01 + l11 + l21 + l02 + l12 + l22) / 9.0;

    float adaptiveSensitivity = 1.0 / max(avgLuminance, 0.05);
    
    // Sobel operators for gradient calculation
    float gx = l00 + 2.0 * l01 + l02 - l20 - 2.0 * l21 - l22;
    float gy = l00 + 2.0 * l10 + l20 - l02 - 2.0 * l12 - l22;
    
    // Calculate gradient magnitude
    float sobelEdge = sqrt(gx * gx + gy * gy) * min(adaptiveSensitivity, 5.0);
    
    // Calculate local variance for additional edge information
    float mean = (l00 + l10 + l20 + l01 + l11 + l21 + l02 + l12 + l22) / 9.0;
    float variance = 
        pow(l00 - mean, 2.0) + pow(l10 - mean, 2.0) + pow(l20 - mean, 2.0) +
        pow(l01 - mean, 2.0) + pow(l11 - mean, 2.0) + pow(l21 - mean, 2.0) +
        pow(l02 - mean, 2.0) + pow(l12 - mean, 2.0) + pow(l22 - mean, 2.0);
    variance /= 9.0;

    variance *= adaptiveSensitivity;
    
    // Combine sobel edge detection with variance for better results
    // Variance helps detect textured regions and subtle transitions
    return max(sobelEdge * 0.5, min(variance * 20.0, 1.0));
}

// Function to calculate adaptive blend factor based on edge strength
float calculateAdaptiveBlendFactor(float baseBlendFactor, float edgeStrength) {
    // Calculate edge-adaptive factor
    // - For low edge strength, keep the original blend factor
    // - For high edge strength, reduce the blend factor to reduce ghosting
    float edgeResponse = smoothstep(0.0, edgeThreshold, edgeStrength);
    float adaptiveBlend = mix(baseBlendFactor, minBlend, 
                            clamp(edgeResponse * edgeResponsiveness, 0.0, 1.0));
                            
    return adaptiveBlend;
}

void main()
{
    // Step 1: Get current pixel color and velocity
    vec3 currentColor = sampleCurrentFrame(TexCoords);
    vec2 velocity = getVelocity(TexCoords);
    
    // Step 2: Calculate previous pixel position
    vec2 previousTexCoords = TexCoords - velocity;
    
    // Step 3: Sample previous frame with this position
    vec3 historyColor = samplePreviousFrame(previousTexCoords);
    
    // Step 4: History color clamping to reduce ghosting
    vec3 minColor, maxColor;
    calculateNeighborhoodMinMax(TexCoords, minColor, maxColor);
    
    // Clamp history color to the neighborhood bounds
    historyColor = clamp(historyColor, minColor, maxColor);
    
    // Calculate edge strength for adaptive blending
    float edgeStrength = calculateEdgeStrength(TexCoords);
    
    // Step 5: Calculate adaptive blend factor based on edge strength
    float adaptiveBlendFactor = calculateAdaptiveBlendFactor(blendFactor, edgeStrength);
    
    // Step 6: Blend current and history color using adaptive blend factor
    vec3 finalColor = mix(currentColor, historyColor, adaptiveBlendFactor);
    
    // Debug mode - visualize the edges we're trying to fix
    if (showEdges) {
        // Adaptive threshold based on local brightness
        float localBrightness = rgb2luma(currentColor);
        float edgeThreshold = mix(0.05, 0.1, min(localBrightness * 5.0, 1.0));
        
        if (edgeStrength > edgeThreshold) {
            // Highlight edges in red
            finalColor = mix(finalColor, vec3(1.0, 0.0, 0.0), min(edgeStrength * 5.0, 0.8));
        }
    }
    
    FragColor = vec4(finalColor, 1.0);
}
