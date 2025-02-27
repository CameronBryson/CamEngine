#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 inverseScreenSize;  // 1.0 / screen dimensions
uniform bool fxaaEnabled;

// FXAA parameters
uniform float EDGE_THRESHOLD_MIN;
uniform float EDGE_THRESHOLD_MAX;
uniform float SUBPIXEL_QUALITY;

// Helper functions
float rgb2luma(vec3 rgb) {
    // Convert RGB to luma (perceived brightness)
    // Using Rec. 709 coefficients for accurate brightness perception
    return dot(rgb, vec3(0.299, 0.587, 0.114));
}

void main() {
    if(!fxaaEnabled) {
        FragColor = texture(screenTexture, TexCoords);
        return;
    }
    
    // Sample center and 4 direct neighbors
    vec3 colorCenter = texture(screenTexture, TexCoords).rgb;
    vec3 colorUp = textureOffset(screenTexture, TexCoords, ivec2(0, 1)).rgb;
    vec3 colorDown = textureOffset(screenTexture, TexCoords, ivec2(0, -1)).rgb;
    vec3 colorLeft = textureOffset(screenTexture, TexCoords, ivec2(-1, 0)).rgb;
    vec3 colorRight = textureOffset(screenTexture, TexCoords, ivec2(1, 0)).rgb;

    // Calculate luma values
    float lumaCenter = rgb2luma(colorCenter);
    float lumaUp = rgb2luma(colorUp);
    float lumaDown = rgb2luma(colorDown);
    float lumaLeft = rgb2luma(colorLeft);
    float lumaRight = rgb2luma(colorRight);

    // Find min and max luma
    float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
    float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

    // Calculate luma range
    float lumaRange = lumaMax - lumaMin;
    
    // ADJUSTED: Lower threshold for more aggressive edge detection
    // Makes the effect more visible by applying it to more edges
    if(lumaRange < max(EDGE_THRESHOLD_MIN * 0.5, lumaMax * EDGE_THRESHOLD_MAX * 0.8)) {
        FragColor = vec4(colorCenter, 1.0);
        return;
    }
    
    // Calculate local contrast for better edge detection
    float lumaUpLeft = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(-1, 1)).rgb);
    float lumaUpRight = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(1, 1)).rgb);
    float lumaDownLeft = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(-1, -1)).rgb);
    float lumaDownRight = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(1, -1)).rgb);
    
    // Compute horizontal and vertical contrast
    float lumaUpDown = lumaUp + lumaDown;
    float lumaLeftRight = lumaLeft + lumaRight;
    
    // Add diagonal samples for better detection
    float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
    float lumaRightCorners = lumaDownRight + lumaUpRight;
    float lumaTopCorners = lumaUpLeft + lumaUpRight;
    float lumaBottomCorners = lumaDownLeft + lumaDownRight;
    
    // Improved edge detection with stronger weights to emphasize edges
    float horizontalEdge = abs(lumaLeftCorners - 2.0 * lumaLeft) + 
                          abs(lumaRightCorners - 2.0 * lumaRight) + 
                          abs(lumaLeftRight - 2.0 * lumaCenter) * 1.5; // Increased weight
                          
    float verticalEdge = abs(lumaTopCorners - 2.0 * lumaUp) + 
                        abs(lumaBottomCorners - 2.0 * lumaDown) + 
                        abs(lumaUpDown - 2.0 * lumaCenter) * 1.5; // Increased weight
    
    bool isHorizontal = horizontalEdge >= verticalEdge;
    
    // Determine positive and negative directions more accurately
    float luma1, luma2;
    if(isHorizontal) {
        luma1 = lumaLeft;
        luma2 = lumaRight;
    } else {
        luma1 = lumaUp;
        luma2 = lumaDown;
    }
    
    float gradient1 = luma1 - lumaCenter;
    float gradient2 = luma2 - lumaCenter;
    
    bool is1Steepest = abs(gradient1) >= abs(gradient2);
    
    // ADJUSTED: Increased gradient scale for stronger effect
    float gradientScaled = 0.20 * max(abs(gradient1), abs(gradient2)); // Reduced from 0.25 for more sensitivity
    
    // Choose step size based on edge direction
    float stepLength = isHorizontal ? inverseScreenSize.y : inverseScreenSize.x;
    
    // Adjust step direction
    float lumaLocalAverage = 0.0;
    
    if(is1Steepest) {
        stepLength = -stepLength;
        lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
    } else {
        lumaLocalAverage = 0.5 * (luma2 + lumaCenter);
    }
    
    // First sample point
    vec2 currentUV = TexCoords;
    if(isHorizontal) {
        currentUV.y += stepLength * 0.5;
    } else {
        currentUV.x += stepLength * 0.5;
    }
    
    // Perpendicular direction sampling
    vec2 offset = isHorizontal ? vec2(inverseScreenSize.x, 0.0) : vec2(0.0, inverseScreenSize.y);
    
    // Calculate sampling points
    vec2 uv1 = currentUV - offset;
    vec2 uv2 = currentUV + offset;
    
    // Sample at ends of line segment
    float lumaEnd1 = rgb2luma(texture(screenTexture, uv1).rgb);
    float lumaEnd2 = rgb2luma(texture(screenTexture, uv2).rgb);
    
    // Check if we're reaching the end of the edge
    lumaEnd1 -= lumaLocalAverage;
    lumaEnd2 -= lumaLocalAverage;
    
    bool reached1 = abs(lumaEnd1) >= gradientScaled;
    bool reached2 = abs(lumaEnd2) >= gradientScaled;
    bool reachedBoth = reached1 && reached2;
    
    // If we haven't reached the end, continue searching
    if(!reachedBoth) {
        currentUV = is1Steepest ? uv1 : uv2;
        
        // ADJUSTED: More iterations for more thorough edge detection
        for(int i = 0; i < 12; i++) { // Increased from 9 to 12
            if(isHorizontal) {
                offset.x *= 0.5;
            } else {
                offset.y *= 0.5;
            }
            
            // Shift current UV and resample
            if(is1Steepest) {
                currentUV -= offset;
            } else {
                currentUV += offset;
            }
            
            float lumaEnd = rgb2luma(texture(screenTexture, currentUV).rgb);
            lumaEnd -= lumaLocalAverage;
            
            // If we've reached the edge, stop
            if(abs(lumaEnd) >= gradientScaled) {
                break;
            }
        }
    }
    
    // Calculate distance to edge
    float distance = isHorizontal ? (TexCoords.x - currentUV.x) : (TexCoords.y - currentUV.y);
    bool isDirection1 = distance < 0.0;
    
    // Final distance is the minimum of the two searches
    if(isDirection1) {
        distance = -distance;
    }
    
    // Edge quality detection
    float edgeThickness = (lumaEnd1 + lumaEnd2) * 0.5;
    
    bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;
    
    // Fix for white artifact - ensure consistency of edge direction and luminance changes
    bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;
    
    // ADJUSTED: Strengthen the effect by increasing the offset
    float finalOffset = correctVariation ? distance * 1.5 : 0.0; // Multiplied by 1.5 to strengthen
    
    // Anti-alias subpixel precision
    float lumaAverage = (1.0/12.0) * (2.0 * (lumaUpDown + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);
    float subPixelOffset1 = clamp((lumaCenter - lumaAverage) / lumaRange, -0.5, 0.5);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
    
    // ADJUSTED: Increased subpixel quality multiplier
    float adjustedSubpixelQuality = SUBPIXEL_QUALITY * 1.5; // Increased effect strength
    finalOffset = max(finalOffset, subPixelOffset2 * adjustedSubpixelQuality);
    
    // Final UV coordinates
    vec2 finalUV = TexCoords;
    if(isHorizontal) {
        finalUV.y += finalOffset * stepLength;
    } else {
        finalUV.x += finalOffset * stepLength;
    }
    
    // Sample final color
    FragColor = vec4(texture(screenTexture, finalUV).rgb, 1.0);
    
    // ADDED: Debug visualization (uncomment to see where FXAA is being applied)
    // This adds a subtle green tint to pixels where FXAA is being applied
    //FragColor.rgb += vec3(0.0, 0.1, 0.0) * finalOffset * 2.0;
}
