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
    
    vec3 brightColor = (brightness > threshold) ? hdrColor : vec3(0.0);
    FragColor = vec4(brightColor, 1.0);
}
