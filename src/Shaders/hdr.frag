// In hdr.frag
#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;
uniform bool hdr;
uniform bool aces;
uniform bool bloom;
uniform float exposure;
uniform float bloomStrength;
uniform float brightness;
uniform float contrast;
uniform float saturation;


// ACES tone mapping function
vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Simple exposure tone mapping
vec3 simpleExposure(vec3 color)
{
    return vec3(1.0) - exp(-color);
}

// Color grading function for adjusting brightness, contrast, and saturation
vec3 colorGrade(vec3 color)
{
    // Brightness adjustment (applied before other operations)
    color += brightness;
    
    // Contrast adjustment
    color = (color - 0.5) * contrast + 0.5;
    
    // Saturation adjustment
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    color = mix(vec3(luminance), color, saturation);
    
    return clamp(color, 0.0, 1.0);
}

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBuffer, TexCoords).rgb;
    
    // Apply bloom if enabled
    if(bloom)
        hdrColor += bloomColor * bloomStrength;

    vec3 result;
    
    if(hdr)
    {
        // Apply exposure adjustment
        hdrColor *= exposure;
        
        // Apply tone mapping based on selected operator
        if(aces)
            result = ACESFilm(hdrColor);
        else
            result = simpleExposure(hdrColor);
        
        // Apply color grading
        result = colorGrade(result);
        
        // Apply gamma correction
        result = pow(result, vec3(1.0 / gamma));
    }
    else
    {
        // Apply color grading even in non-HDR mode
        result = colorGrade(hdrColor);
        
        // Apply gamma correction
        result = pow(result, vec3(1.0 / gamma));
    }
    
    FragColor = vec4(result, 1.0);
}
