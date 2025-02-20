// In hdr.frag
#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;
uniform bool hdr;
uniform bool bloom;
uniform float exposure;
uniform float bloomStrength;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBuffer, TexCoords).rgb;
    
    if(bloom)
        hdrColor += bloomColor * bloomStrength;

    if(hdr)
    {
        // Exposure tone mapping
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // Gamma correction
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}
