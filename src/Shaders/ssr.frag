#version 460 core
precision highp float;

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gNormalMetallic;  // RGB: World space normal, A: Metallic. Normal is in world space [0,1]
uniform sampler2D gVelocityReflective; // RG: Screen-space Velocity, B Reflective , A Unused. Reflective is scalar
uniform sampler2D gDepth; // Depth from G-buffer, window depth space 0 to 1
uniform sampler2D gSceneColor; // Scene color from G-buffer.  Color for each pixel

layout(std140, binding = 0) uniform CameraBlock {
    mat4 view;                  
    mat4 projection;            
    vec4 cameraPos;              
    mat4 previousView;           
    mat4 previousProjection;     
};
uniform bool test;

//This check is in [0,1] space
bool isRayOnScreen(vec2 ray)
{
    return ray.x >= 0 && ray.x <= 1 && ray.y >= 0 && ray.y <= 1;
}
void main()
{
    ivec2 screenSize = textureSize(gSceneColor, 0);
    vec3 color = texture(gSceneColor, TexCoord).rgb;
    float reflectivity = texture(gVelocityReflective, TexCoord).b;
    if(reflectivity < 0.1)
    {
        FragColor = vec4(color, 1);
        return;
    }
    // Unpack the normal from [0,1] to [-1,1] space
    vec3 normal = texture(gNormalMetallic, TexCoord).rgb * 2.0 - 1.0;
    vec3 normalInVS = normalize(mat3(view) * normal); // Transform normal to view space
    float depth = texture(gDepth, TexCoord).r;

    //COMPUTE POS AND REFLECTION

    //Clip space and centering on pixel
    //Metal clip space depth is 0 to 1 and opengl is -1 to 1
    vec2 texCoordAdjusted = TexCoord;
    vec4 fragPosInCS = vec4(texCoordAdjusted * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    
    vec4 fragPosInVS = inverse(projection) * fragPosInCS;
    fragPosInVS /= fragPosInVS.w;

    //Camera to fragment vector in view space
    vec3 viewDirInVS = normalize(fragPosInVS.xyz);
    vec3 reflectionDirInVS = reflect(viewDirInVS, normalInVS);
    
    //Create reflection end point in view space
    vec3 reflectionEndPosInVS = fragPosInVS.xyz + reflectionDirInVS * 1000.0;
    
    //Project back to clip space
    vec4 reflectionEndPosInCS = projection * vec4(reflectionEndPosInVS, 1.0);
    reflectionEndPosInCS /= reflectionEndPosInCS.w;
    
    //Reflection direction in clip space
    vec3 reflectionDirInCS = normalize(reflectionEndPosInCS.xyz - fragPosInCS.xyz);
    
    //Transform to texture space [0,1]
    vec3 fragPosInTS = vec3(fragPosInCS.xy * 0.5 + 0.5, fragPosInCS.z * 0.5 + 0.5);
    vec3 reflectionDirInTS = vec3(reflectionDirInCS.xy * 0.5, reflectionDirInCS.z * 0.5);
    
    //Maximum distance to trace a ray
    float maxDistance = 1.0;
    if (reflectionDirInTS.x != 0.0) {
        float distX = reflectionDirInTS.x > 0.0 ? 
            (1.0 - fragPosInTS.x) / reflectionDirInTS.x : 
            -fragPosInTS.x / reflectionDirInTS.x;
        maxDistance = min(maxDistance, distX);
    }
    
    if (reflectionDirInTS.y != 0.0) {
        float distY = reflectionDirInTS.y > 0.0 ? 
            (1.0 - fragPosInTS.y) / reflectionDirInTS.y : 
            -fragPosInTS.y / reflectionDirInTS.y;
        maxDistance = min(maxDistance, distY);
    }
    
    if (reflectionDirInTS.z != 0.0) {
        float distZ = reflectionDirInTS.z > 0.0 ? 
            (1.0 - fragPosInTS.z) / reflectionDirInTS.z : 
            -fragPosInTS.z / reflectionDirInTS.z;
        maxDistance = min(maxDistance, distZ);
    }

    // FIND INTERSECTION LINEAR
    vec3 reflectionEndPosInTS = fragPosInTS + reflectionDirInTS * maxDistance;
    
    ivec2 fragScreenPos = ivec2(fragPosInTS.xy * screenSize);
    ivec2 endScreenPos = ivec2(reflectionEndPosInTS.xy * screenSize);
    ivec2 dp2 = endScreenPos - fragScreenPos;
    
    int max_dist = max(abs(dp2.x), abs(dp2.y));
    max_dist = min(max_dist, 100); // Limit iteration count
    
    vec3 dp = (reflectionEndPosInTS - fragPosInTS) / float(max(max_dist, 1));
    
    vec3 rayPosInTS = fragPosInTS;
    vec3 rayDirInTS = dp;
    
    int hitIndex = -1;
    for(int i = 0; i < max_dist; i++)
    {
        rayPosInTS += rayDirInTS;
        
        // Check if ray is on screen
        if(!isRayOnScreen(rayPosInTS.xy))
            break;
        
        float depthSample = texture(gDepth, rayPosInTS.xy).r;
        float thickness = rayPosInTS.z - depthSample;
        
        if(thickness > 0.0 && thickness < 0.1) // Depth test with tolerance
        {
            hitIndex = i;
            break;
        }
    }
    
    bool intersected = hitIndex >= 0;
    float intensity = intersected ? 1.0 : 0.0; 
    
    vec3 sampledColor = texture(gSceneColor, rayPosInTS.xy).rgb;
    vec3 finalColor = mix(color, sampledColor, intensity);
    
    FragColor = vec4(finalColor, 1.0);
}
