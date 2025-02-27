// motionblur.frag
#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorTexture;    // HDR color buffer
uniform sampler2D velocityTexture; // Velocity from G-buffer
uniform float blurStrength = 1.0;  // Adjustable blur strength
uniform int numSamples = 8;        // Number of samples for the blur

void main()
{
    vec2 velocity = texture(velocityTexture, TexCoords).rg;
    // Scale velocity by blur strength
    velocity *= blurStrength;
    
    vec4 color = texture(colorTexture, TexCoords);
    
    // Only blur if there is movement
    float velocityLength = length(velocity);
    if(velocityLength > 0.0) {
        for(int i = 1; i < numSamples; i++) {
            // Sample along velocity vector
            vec2 offset = velocity * (float(i) / float(numSamples - 1) - 0.5);
            color += texture(colorTexture, TexCoords + offset);
        }
        color /= float(numSamples);
    }
    
    FragColor = color;
}
