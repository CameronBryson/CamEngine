#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D currentFrame;
uniform sampler2D previousFrame; 
uniform sampler2D velocityMap;
uniform float blendFactor;
uniform vec2 resolution;

bool debugEdges = false;

vec2 getVelocity(vec2 uv) {
    return texture(velocityMap, uv).rg;
}

vec3 sampleCurrentFrame(vec2 uv) {
    return texture(currentFrame, uv).rgb;
}

vec3 samplePreviousFrame(vec2 uv) {
    return texture(previousFrame, uv).rgb;
}

// Function to calculate neighborhood bounds for history clamping
void calculateNeighborhoodMinMax(vec2 uv, out vec3 minColor, out vec3 maxColor) {
    vec3 center = sampleCurrentFrame(uv);
    
    // Sample neighboring pixels in a cross pattern
    vec3 top = sampleCurrentFrame(uv + vec2(0, 1) / resolution);
    vec3 right = sampleCurrentFrame(uv + vec2(1, 0) / resolution);
    vec3 bottom = sampleCurrentFrame(uv + vec2(0, -1) / resolution);
    vec3 left = sampleCurrentFrame(uv + vec2(-1, 0) / resolution);
    
    // Calculate min/max bounds
    minColor = min(center, min(top, min(right, min(bottom, left))));
    maxColor = max(center, max(top, max(right, max(bottom, left))));
}

// Calculate edge strength based on color differences
float calculateEdgeStrength(vec2 uv) {
    vec3 center = sampleCurrentFrame(uv);
    
    vec3 top = sampleCurrentFrame(uv + vec2(0, 1) / resolution);
    vec3 right = sampleCurrentFrame(uv + vec2(1, 0) / resolution);
    vec3 bottom = sampleCurrentFrame(uv + vec2(0, -1) / resolution);
    vec3 left = sampleCurrentFrame(uv + vec2(-1, 0) / resolution);
    
    // Calculate color differences
    float topDiff = length(center - top);
    float rightDiff = length(center - right);
    float bottomDiff = length(center - bottom);
    float leftDiff = length(center - left);
    
    // Return maximum difference as edge strength
    return max(max(topDiff, rightDiff), max(bottomDiff, leftDiff));
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
    
    // Step 5: Blend current and history color
    vec3 finalColor = mix(currentColor, historyColor, blendFactor);
    
    // Calculate edge strength for debugging
    float edgeStrength = calculateEdgeStrength(TexCoords);
    
    // Debug mode - visualize the edges we're trying to fix
    if (debugEdges) {
        // Threshold for edge detection
        float edgeThreshold = 0.1;
        
        if (edgeStrength > edgeThreshold) {
            // Highlight edges in red
            finalColor = mix(finalColor, vec3(1.0, 0.0, 0.0), min(edgeStrength * 5.0, 0.8));
        }
    }
    
    FragColor = vec4(finalColor, 1.0);
}
