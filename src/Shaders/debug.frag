#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform sampler2D depthMap;

void main()
{
    // Calculate normalized device coordinates
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get depth of current fragment
    float currentDepth = projCoords.z;
    
    // Get closest depth from shadow map
    float closestDepth = texture(depthMap, projCoords.xy).r;
    
    // Calculate basic lighting
    vec3 normal = normalize(Normal);
    vec3 lightDir = vec3(0.0, 1.0, 0.0); // Assuming directional light
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Visualize depth with simple diffuse lighting
    vec3 color = vec3(1.0 - currentDepth);
    color *= diff * 0.7 + 0.3; // Add some ambient
    
    FragColor = vec4(color, 1.0);
}
