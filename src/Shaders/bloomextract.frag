#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float threshold;

// Convert RGB to luminance using perceptual weights
float getLuminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main()
{
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    
    // Calculate brightness using luminance
    float brightness = getLuminance(hdrColor);
    
    // Apply soft threshold
    float softness = 0.1; // Controls the smoothness of the threshold cutoff
    float knee = threshold + softness;
    float soft = brightness - threshold;
    soft = clamp(soft, 0.0, softness);
    soft = soft * soft * (3.0 - 2.0 * soft) / softness; // Smooth interpolation
    
    // If brightness is higher than threshold, output the color
    vec3 brightColor = mix(vec3(0.0), hdrColor, 
                          brightness < threshold ? 0.0 : 
                          brightness > knee ? 1.0 : soft);
    
    FragColor = vec4(brightColor, 1.0);
}
