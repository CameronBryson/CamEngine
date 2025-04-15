#version 460 core
precision highp float;

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gNormalMetallic;  // RGB: World space normal, A: Metallic. Normal is in world space [0,1]
uniform sampler2D gVelocityReflective; // RG: Screen-space Velocity, B Reflective , A Unused. Reflective is scalar
uniform sampler2D gDepth; // Depth from G-buffer, window depth space 0 to 1
uniform sampler2D gSceneColor; // Scene color from G-buffer.  Color for each pixel

// SSR uniform variables for adjusting reflection properties
uniform float uReflectionIntensity = 1.0;     // Overall reflection intensity multiplier
uniform float uRayThickness = 0.1;            // Maximum thickness for ray intersection (replaces MAX_THICKNESS)
uniform int uMaxRaySteps = 100;               // Maximum number of ray steps
uniform float uMinReflectivity = 0.1;         // Minimum reflectivity threshold
uniform float uReflectionFalloffDistance = 1000.0; // Controls how far reflections can go in view space
uniform float uRayOffset = 1.0;               // Controls ray starting position offset to avoid self-intersection

layout(std140, binding = 0) uniform CameraBlock {
    vec4 cameraPos;              
    mat4 view;                  
    mat4 projection;
    mat4 viewProjection;
    mat4 inverseView;
    mat4 inverseProjection;
    mat4 inverseViewProjection;
    mat4 previousView;           
    mat4 previousProjection;
    mat4 previousViewProjection;
};

bool isRayOnScreen(vec2 ray)
{
    return ray.x >= 0.0 && ray.x <= 1.0 && ray.y >= 0.0 && ray.y <= 1.0;
}

// Find intersection using batch sampling approach
int FindIntersection_Linear(vec3 rayStartPos, vec3 rayDir, float maxDistance, out vec3 intersection)
{
    vec3 rayEndPos = rayStartPos + rayDir * maxDistance;
    ivec2 screenSize = textureSize(gSceneColor, 0);
    
    ivec2 startPixel = ivec2(rayStartPos.xy * screenSize);
    ivec2 endPixel = ivec2(rayEndPos.xy * screenSize);
    ivec2 delta = endPixel - startPixel;
    
    int max_dist = max(abs(delta.x), abs(delta.y));
    max_dist = min(max_dist, uMaxRaySteps); // Use uniform for max iteration count
    
    vec3 step = (rayEndPos - rayStartPos) / float(max(max_dist, 1));
    
    // Move ray origin slightly forward to avoid self-intersection, using uniform offset
    vec3 rayPos = rayStartPos + step * uRayOffset;
    vec3 originalRayPos = rayPos;
    
    int hitIndex = -1;
    
    // Batch sampling with 4 samples per iteration
    for(int i = 0; i < max_dist && i < uMaxRaySteps; i += 4)
    {
        // Calculate 4 positions
        vec3 pos0 = rayPos + step * 0.0;
        vec3 pos1 = rayPos + step * 1.0;
        vec3 pos2 = rayPos + step * 2.0;
        vec3 pos3 = rayPos + step * 3.0;
        
        // Early screen bounds check
        if(!isRayOnScreen(pos3.xy))
            break;
            
        // Sample depths in reverse order for better cache coherency
        float depth3 = texture(gDepth, pos3.xy).r;
        float depth2 = texture(gDepth, pos2.xy).r;
        float depth1 = texture(gDepth, pos1.xy).r;
        float depth0 = texture(gDepth, pos0.xy).r;
        
        // Test intersections with conditional assignments, using uniform ray thickness
        float thickness3 = pos3.z - depth3;
        hitIndex = (thickness3 >= 0.0 && thickness3 < uRayThickness) ? (i + 3) : hitIndex;
        
        float thickness2 = pos2.z - depth2;
        hitIndex = (thickness2 >= 0.0 && thickness2 < uRayThickness) ? (i + 2) : hitIndex;
        
        float thickness1 = pos1.z - depth1;
        hitIndex = (thickness1 >= 0.0 && thickness1 < uRayThickness) ? (i + 1) : hitIndex;
        
        float thickness0 = pos0.z - depth0;
        hitIndex = (thickness0 >= 0.0 && thickness0 < uRayThickness) ? (i + 0) : hitIndex;
        
        // Break early if intersection found
        if(hitIndex != -1) 
            break;
            
        rayPos = pos3 + step;
    }
    
    // Calculate intersection position if hit found
    intersection = (hitIndex >= 0) ? (originalRayPos + step * float(hitIndex)) : rayStartPos;
    return hitIndex;
}

void main()
{
    ivec2 screenSize = textureSize(gSceneColor, 0);
    vec3 color = texture(gSceneColor, TexCoord).rgb;
    float reflectivity = texture(gVelocityReflective, TexCoord).b;
    
    // Early-out for non-reflective surfaces, using uniform threshold
    if(reflectivity < uMinReflectivity)
    {
        FragColor = vec4(color, 1.0);
        return;
    }
    
    // Unpack the normal from [0,1] to [-1,1] space
    vec3 normal = texture(gNormalMetallic, TexCoord).rgb * 2.0 - 1.0;
    vec3 normalInVS = normalize(mat3(view) * normal);
    float depth = texture(gDepth, TexCoord).r;

    // Compute positions in various spaces
    vec4 fragPosInCS = vec4(TexCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 fragPosInVS = inverse(projection) * fragPosInCS;
    fragPosInVS /= fragPosInVS.w;

    // Calculate reflection vector in view space
    vec3 viewDirInVS = normalize(fragPosInVS.xyz);
    vec3 reflectionDirInVS = reflect(viewDirInVS, normalInVS);
    
    // Project reflection vector to find end point, using uniform for reflection distance
    vec3 reflectionEndPosInVS = fragPosInVS.xyz + reflectionDirInVS * uReflectionFalloffDistance;
    vec4 reflectionEndPosInCS = projection * vec4(reflectionEndPosInVS, 1.0);
    reflectionEndPosInCS /= reflectionEndPosInCS.w;
    
    // Calculate reflection direction in clip space
    vec3 reflectionDirInCS = normalize(reflectionEndPosInCS.xyz - fragPosInCS.xyz);
    
    // Transform to texture space [0,1]
    vec3 fragPosInTS = vec3(fragPosInCS.xy * 0.5 + 0.5, fragPosInCS.z * 0.5 + 0.5);
    vec3 reflectionDirInTS = vec3(reflectionDirInCS.xy * 0.5, reflectionDirInCS.z * 0.5);
    
    // Calculate maximum trace distance
    float maxDistance = 1.0;
    maxDistance = (reflectionDirInTS.x != 0.0) ? 
        min(maxDistance, reflectionDirInTS.x > 0.0 ? 
            (1.0 - fragPosInTS.x) / reflectionDirInTS.x : 
            -fragPosInTS.x / reflectionDirInTS.x) : maxDistance;
            
    maxDistance = (reflectionDirInTS.y != 0.0) ? 
        min(maxDistance, reflectionDirInTS.y > 0.0 ? 
            (1.0 - fragPosInTS.y) / reflectionDirInTS.y : 
            -fragPosInTS.y / reflectionDirInTS.y) : maxDistance;
            
    maxDistance = (reflectionDirInTS.z != 0.0) ? 
        min(maxDistance, reflectionDirInTS.z > 0.0 ? 
            (1.0 - fragPosInTS.z) / reflectionDirInTS.z : 
            -fragPosInTS.z / reflectionDirInTS.z) : maxDistance;

    // Find intersection using optimized batch sampling
    vec3 intersection;
    int hitIndex = FindIntersection_Linear(fragPosInTS, reflectionDirInTS, maxDistance, intersection);
    
    // Sample color and apply fallback
    vec3 sampledColor = (hitIndex >= 0) ? texture(gSceneColor, intersection.xy).rgb : color;
    
    // Mix reflection with original color based on reflectivity, hit success, and uniform intensity
    vec3 finalColor = mix(color, sampledColor, reflectivity * float(hitIndex >= 0) * uReflectionIntensity);
    
    FragColor = vec4(finalColor, 1.0);
}
