#version 460 core
precision highp float;

// G-Buffer outputs
layout (location = 0) out vec4 gAlbedoAO;      // RGB: Albedo, A: Ambient Occlusion
layout (location = 1) out vec4 gNormalMetallic; // RGB: World space normal, A: Metallic
layout (location = 2) out vec4 gRoughEmissive;  // R: Roughness, GBA: Emissive
layout (location = 3) out vec4 gVelocityReflective; // RG: Screen-space Velocity, B Reflective , A Unused

// Inputs from vertex shader
layout (location = 0) in vec2 TexCoord;
layout (location = 1) in vec3 FragPos;
layout (location = 2) in mat3 TBN; //TBN uses 2,3,4
layout (location = 5) in vec4 ClipPos;
layout (location = 6) in vec4 PrevClipPos;

// Material struct (same as your PBR shader)
struct Material {
    bool hasAlbedoMap;
    bool hasNormalMap;
    bool hasMetallicMap;
    bool hasRoughnessMap;
    bool hasMetalRoughMap;
    bool hasAOMap;
    bool hasEmissiveMap;
    bool hasOpacityMap;
    bool hasDisplacementMap;

    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D metalRoughMap;
    sampler2D AOMap;
    sampler2D emissiveMap;
    sampler2D opacityMap;
    sampler2D displacementMap;

    vec4  albedo;
    float metallic;
    float roughness;
    float opacity;
    vec3  emissiveColor;
    float emissiveIntensity;
    float displacementScale;
    float reflectivity;
};

uniform Material material;
uniform bool normalmapping;

vec3 getNormalFromMap()
{
    if(material.hasNormalMap && normalmapping) {
        // Sample normal from texture

        vec3 tangentNormal = texture(material.normalMap, TexCoord).rgb;
        tangentNormal = tangentNormal * 2.0f - 1.0f;

        vec3 worldNormal = TBN * tangentNormal;
        
        // Only normalize once at the end
        return normalize(worldNormal);
    }
    
    return normalize(TBN[2]);
}




void main()
{
    
    // Early discard for transparent pixels
    float alpha = material.opacity;
    if (material.hasOpacityMap) {
        alpha *= texture(material.opacityMap, TexCoord).r;
    }
    else if (material.hasAlbedoMap) {
        alpha *= texture(material.albedoMap, TexCoord).a;
    }
    else {
        alpha *= material.albedo.a;
    }
    
    if (alpha < 0.1) {
        discard;
    }

    // 1. Albedo and AO
    vec3 albedo = material.hasAlbedoMap ? texture(material.albedoMap, TexCoord).rgb : material.albedo.rgb;
    float ao = material.hasAOMap ? texture(material.AOMap, TexCoord).r : 1.0;
    gAlbedoAO = vec4(albedo, ao);

    // 2. Normal and Metallic
    vec3 normal = getNormalFromMap();
    float metallic = material.metallic;
    if (material.hasMetalRoughMap) {
        metallic = texture(material.metalRoughMap, TexCoord).b;
    }
    else if (material.hasMetallicMap) {
        metallic = texture(material.metallicMap, TexCoord).r;
    }
    gNormalMetallic = vec4(normal * 0.5 + 0.5, metallic);

    // 3. Roughness and Emissive
    float roughness = material.roughness;
    if (material.hasMetalRoughMap) {
        roughness = texture(material.metalRoughMap, TexCoord).g;
    }
    else if (material.hasRoughnessMap) {
        roughness = texture(material.roughnessMap, TexCoord).r;
    }
    roughness = clamp(roughness, 0.05, 1.0);

    vec3 emissive = material.emissiveColor * material.emissiveIntensity;
    if (material.hasEmissiveMap) {
        emissive *= texture(material.emissiveMap, TexCoord).rgb;
    }

    gRoughEmissive = vec4(roughness, emissive);

   // Calculate the positions in NDC space
    vec2 currentPosNDC = (ClipPos.xy / ClipPos.w);
    vec2 prevPosNDC = (PrevClipPos.xy / PrevClipPos.w);
    
    vec2 velocity = currentPosNDC - prevPosNDC;
    float velocityLength = length(velocity);
    //Account for jittering
    if(velocityLength < 0.001)
    {
        velocity = vec2(0.0);
    }
    // Calculate reflectivity based on Fresnel equation approximation
    float F0 = material.reflectivity; // Base reflectivity at normal incidence
    float dielectricReflectivity = F0;
    float metallicReflectivity = 1.0; // Metals reflect almost all light
    float reflectivity = mix(dielectricReflectivity, metallicReflectivity, metallic);

    // Modify reflectivity based on roughness (rougher surfaces reflect less clearly)
    reflectivity *= max(0.2, 1.0 - roughness * 0.3);

    // Ensure reflectivity stays in reasonable range
    reflectivity = clamp(reflectivity, 0.0, 1.0);

    gVelocityReflective = vec4(velocity, reflectivity, 0);
}
