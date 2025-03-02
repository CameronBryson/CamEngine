#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;
uniform float scatteringCoefficient; // Controls how much light spreads
uniform int kernelSize; // Dynamic kernel size based on brightness

// Physically-based Gaussian function
float gaussian(float x, float strength)
{
    // 2.506628 = sqrt(2*PI)
    return exp(-(x*x) / (2.0 * strength*strength)) / (strength * 2.506628);
}

void main()
{
    vec2 texSize = textureSize(image, 0);
    vec2 texelSize = 1.0 / texSize;
    vec3 result = vec3(0.0);
    float totalWeight = 0.0;
    
    // Sample center pixel to determine blur radius
    vec3 centerColor = texture(image, TexCoords).rgb;
    float centerBrightness = max(dot(centerColor, vec3(0.2126, 0.7152, 0.0722)), 0.01);
    
    // Stronger blur for brighter pixels - physically based light scattering
    float blurRadius = clamp(10.0 * centerBrightness * scatteringCoefficient, 0.5, 20.0);
    
    // Dynamic sampling with minimum to ensure we always have some samples
    int samples = max(3, min(kernelSize, int(blurRadius * 2.0) + 1));
    
    // Perform physically-based blur
    if(horizontal) {
        for(int i = -samples; i <= samples; i++) {
            float offset = float(i);
            float weight = gaussian(offset, blurRadius);
            result += texture(image, TexCoords + vec2(texelSize.x * offset, 0.0)).rgb * weight;
            totalWeight += weight;
        }
    }
    else {
        for(int i = -samples; i <= samples; i++) {
            float offset = float(i);
            float weight = gaussian(offset, blurRadius);
            result += texture(image, TexCoords + vec2(0.0, texelSize.y * offset)).rgb * weight;
            totalWeight += weight;
        }
    }
    
    // Normalize with safety check
    result = totalWeight > 0.0 ? result / totalWeight : vec3(0.0);
    
    FragColor = vec4(result, 1.0);
}
