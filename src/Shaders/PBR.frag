#version 450 core

// ------------------------------------------------------------------------------------
// Output
// ------------------------------------------------------------------------------------
out vec4 FragColor;

// ------------------------------------------------------------------------------------
// Inputs from Vertex Shader
// ------------------------------------------------------------------------------------
in vec2 TexCoord;
in vec3 FragPos;    // World-space fragment position
in mat3 TBN;        // Tangent, Bitangent, Normal (for normal mapping)

// ------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------
const float PI = 3.14159265359;
const int MAX_POINT_LIGHTS       = 25;
const int MAX_SPOT_LIGHTS        = 25;
const int MAX_DIRECTIONAL_LIGHTS = 25;

// ------------------------------------------------------------------------------------
// Uniform Blocks (std140 layout)
// ------------------------------------------------------------------------------------

// Camera data block (binding = 0)
layout(std140, binding = 0) uniform CameraBlock {
    mat4 view;
    mat4 projection;
    vec4 cameraPos;  // xyz = camera position; w = padding
};

// Light data block (binding = 1)
struct PointLightData {
    vec4 position;     // xyz = position, w unused
    vec4 ambient;      // rgb = ambient color, w unused
    vec4 diffuse;      // rgb = diffuse color, w unused
    vec4 specular;     // rgb = specular color, w unused
    vec4 attenuation;  // x = constant, y = linear, z = quadratic, w unused
};

struct SpotLightData {
    vec4 position;    // xyz = position, w unused
    vec4 direction;   // xyz = direction, w unused
    vec4 ambient;     // rgb = ambient color, w unused
    vec4 diffuse;     // rgb = diffuse color, w unused
    vec4 specular;    // rgb = specular color, w unused
    vec4 attenuation; // x = constant, y = linear, z = quadratic, w unused
    vec4 cutoffs;     // x = inner cutoff (cos), y = outer cutoff (cos), z/w unused
    mat4 lightSpaceMatrix; // Used for shadow mapping.
};

struct DirectionalLightData {
    vec4 direction;   // xyz = light direction, w unused
    vec4 ambient;     // rgb = ambient color, w unused
    vec4 diffuse;     // rgb = diffuse color, w unused
    vec4 specular;    // rgb = specular color, w unused
    mat4 lightSpaceMatrix; // Used for shadow mapping.
};

layout(std140, binding = 1) uniform LightBlock {
    // counts.x = number of point lights,
    // counts.y = spot lights,
    // counts.z = directional lights.
    ivec4 counts;
    PointLightData       pointLights[MAX_POINT_LIGHTS];
    SpotLightData        spotLights[MAX_SPOT_LIGHTS];
    DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

// ------------------------------------------------------------------------------------
// Material Uniform (not in a UBO)
// ------------------------------------------------------------------------------------
struct Material {
    bool hasAlbedoMap;
    bool hasNormalMap;
    bool hasMetallicMap;
    bool hasRoughnessMap;
    bool hasMetalRoughMap;
    bool hasAOMap;
    bool hasEmissiveMap;

    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D metalRoughMap;
    sampler2D AOMap;
    sampler2D emissiveMap;

    vec4 albedo;
    float metallic;
    float roughness;
    float AO;
};

uniform Material material;

// ------------------------------------------------------------------------------------
// IBL Samplers
// ------------------------------------------------------------------------------------
uniform samplerCube irradianceMap;   // Diffuse irradiance
uniform samplerCube prefilterMap;    // Prefiltered specular environment
uniform sampler2D  brdfLUT;          // 2D LUT for split–sum IBL

// ------------------------------------------------------------------------------------
// Shadow Mapping for Directional Lights
// ------------------------------------------------------------------------------------
uniform sampler2D directionalShadowMap;
uniform sampler2D spotShadowMap;

// ------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  fresnelSchlick(float cosTheta, vec3 F0);
vec3  fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3  getNormalFromMap();

void mainPBR(out vec3 outColor);
// Replace both calculateDirectionalShadow and calculateSpotShadow with this single function:
float calculateShadow(vec3 worldPos, vec3 normal, vec3 lightDir, mat4 lightSpaceMatrix, sampler2D shadowMap)
{
    // Transform the world position into light space and normalize coordinates to [0,1]
    vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // If outside the light's frustum, consider it fully lit
    if(projCoords.z > 1.0)
        return 1.0;
    
    // Compute dynamic bias based on angle between normal and light direction
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    // Percentage-Closer Filtering (PCF)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias) > pcfDepth ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}



// ------------------------------------------------------------------------------------
// Main Entry Point
// ------------------------------------------------------------------------------------
void main()
{
    vec3 color;

    // Normal rendering path
    mainPBR(color);

    // Reinhard tone mapping.
    color = color / (color + vec3(1.0));
    // Gamma correction (linear to sRGB).
    color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0);
}

// ------------------------------------------------------------------------------------
// Core PBR Implementation
// ------------------------------------------------------------------------------------
void mainPBR(out vec3 outColor)
{
    // 1) Base Albedo.
    vec3 albedoColor = material.albedo.rgb;
    if (material.hasAlbedoMap) {
        albedoColor = texture(material.albedoMap, TexCoord).rgb;
        // Make sure to remove sRGB if the texture is in sRGB space
        albedoColor = pow(albedoColor, vec3(2.2));
    }

    // 2) Normal Mapping.
    vec3 N = getNormalFromMap();

    // 3) Metallic & Roughness.
    float metallicValue = material.metallic;
    float roughnessValue = material.roughness;
    if (material.hasMetalRoughMap) {
        vec3 mr = texture(material.metalRoughMap, TexCoord).rgb;
        roughnessValue = mr.g;
        metallicValue  = mr.b;
    } else {
        if (material.hasMetallicMap)
            metallicValue = texture(material.metallicMap, TexCoord).r;
        if (material.hasRoughnessMap)
            roughnessValue = texture(material.roughnessMap, TexCoord).r;
    }
    roughnessValue = clamp(roughnessValue, 0.05, 1.0);

    // 4) Ambient Occlusion.
    float aoValue = material.AO;
    if (material.hasAOMap)
        aoValue = texture(material.AOMap, TexCoord).r;

    // 5) Emissive Color.
    vec3 emissiveColor = vec3(0.0);
    if (material.hasEmissiveMap)
        emissiveColor = texture(material.emissiveMap, TexCoord).rgb;

    // 6) View and Reflection.
    vec3 V  = normalize(cameraPos.xyz - FragPos);
    vec3 R  = reflect(-V, N);
    vec3 F0 = mix(vec3(0.04), albedoColor, metallicValue);

    // 7) Direct Lighting Contribution.
    vec3 Lo = vec3(0.0);

    // ------------------------------------------------------------------------
    // POINT LIGHTS
    // ------------------------------------------------------------------------
    for (int i = 0; i < counts.x; i++)
    {
        PointLightData pl = pointLights[i];

        // Light vector
        vec3 L = pl.position.xyz - FragPos;
        float dist  = length(L);
        L           = normalize(L);

        // PBR lighting terms
        vec3 H      = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= (1.0 - metallicValue);

            vec3 diffuse = kD * albedoColor / PI;

            // Attenuation
            float attenuation = 1.0 / (pl.attenuation.x +
                                       pl.attenuation.y * dist +
                                       pl.attenuation.z * (dist * dist));

            // Final contribution
            vec3 radiance = pl.diffuse.rgb; // you could multiply by intensity or other factor if you wish
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL * attenuation;

            Lo += lightContrib;
        }
    }

    // ------------------------------------------------------------------------
    // SPOT LIGHTS
    // ------------------------------------------------------------------------
    for (int i = 0; i < counts.y; i++)
    {
        SpotLightData sl = spotLights[i];

        // Light vector
        vec3 L = sl.position.xyz - FragPos;
        float dist = length(L);
        L          = normalize(L);

        // Cosine of the angle between L and the spot direction
        float theta     = dot(L, normalize(-sl.direction.xyz));
        float epsilon   = sl.cutoffs.x - sl.cutoffs.y;
        float intensity = clamp((theta - sl.cutoffs.y) / epsilon, 0.0, 1.0);

        // PBR lighting
        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0 && intensity > 0.0)
        {
            float shadowFactor = calculateShadow(FragPos, N, L, sl.lightSpaceMatrix, spotShadowMap);
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= (1.0 - metallicValue);

            vec3 diffuse = kD * albedoColor / PI;

            // Attenuation
            float attenuation = 1.0 / (sl.attenuation.x +
                                       sl.attenuation.y * dist +
                                       sl.attenuation.z * dist * dist);

            vec3 radiance = sl.diffuse.rgb;

            // Final contribution
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL * attenuation * intensity * shadowFactor;
            Lo += lightContrib;
        }
    }

    // ------------------------------------------------------------------------
    // DIRECTIONAL LIGHTS + SHADOWS
    // ------------------------------------------------------------------------
    for (int i = 0; i < counts.z; i++)
    {
        // Direction is stored as directionalLights[i].direction
        vec3 lightDir = normalize(-directionalLights[i].direction.xyz); 
        float NdotL   = max(dot(N, lightDir), 0.0);

        if (NdotL > 0.0)
        {
            // Shadow factor (1.0 = lit, 0.0 = fully in shadow)
            float shadowFactor = calculateShadow(FragPos, N, lightDir, directionalLights[i].lightSpaceMatrix, directionalShadowMap);

            // PBR lighting
            vec3 H = normalize(V + lightDir);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, lightDir, roughnessValue);
            vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= (1.0 - metallicValue);

            vec3 diffuse = kD * albedoColor / PI;

            // Ambient/diffuse/similar stored in the directional light
            vec3 radiance = directionalLights[i].diffuse.rgb;

            // Combine
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL * shadowFactor;
            Lo += lightContrib;
        }
    }

    // ------------------------------------------------------------------------
    // IMAGE-BASED LIGHTING (IBL)
    // ------------------------------------------------------------------------
    // Fresnel for IBL
    vec3 F_IBL = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughnessValue);

    // kS is the ratio of specular reflection
    vec3 kS = F_IBL;
    // kD is diffuse reflection
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);

    // Diffuse IBL
    vec3 irradiance   = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL   = irradiance * albedoColor;

    // Specular IBL
    vec3 Rdir             = reflect(-V, N);
    float NdotV           = max(dot(N, V), 0.0);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, Rdir, roughnessValue * MAX_REFLECTION_LOD).rgb;
    vec2 brdf             = texture(brdfLUT, vec2(NdotV, roughnessValue)).rg;
    vec3 specularIBL      = prefilteredColor * (F_IBL * brdf.x + brdf.y);

    // Combine with AO
    vec3 ambientIBL = (kD * diffuseIBL + specularIBL) * aoValue;
    ambientIBL *= 0.1f;
    //ambientIBL = vec3(0.0);

    // ------------------------------------------------------------------------
    // Final Composition
    // ------------------------------------------------------------------------
    vec3 finalColor = ambientIBL + Lo + emissiveColor;
    outColor = finalColor;
}

// ------------------------------------------------------------------------------------
// Helper Functions for PBR
// ------------------------------------------------------------------------------------
vec3 getNormalFromMap()
{
    if (material.hasNormalMap) {
        vec3 tNormal = texture(material.normalMap, TexCoord).rgb;
        tNormal = tNormal * 2.0 - 1.0; // Map [0,1] to [-1,1]
        return normalize(TBN * tNormal);
    }
    // If no normal map, return the interpolated normal (the 3rd column of TBN).
    return normalize(TBN[2]);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    // k = (r+1)^2 / 8
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    // Allows F to fade to 1 as the roughness increases
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) *
           pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
