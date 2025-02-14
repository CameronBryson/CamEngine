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
    vec4 cutoffs;     // x = inner cutoff, y = outer cutoff, z/w unused
};

struct DirectionalLightData {
    vec4 direction;   // xyz = light direction, w unused
    vec4 ambient;     // rgb = ambient color, w unused
    vec4 diffuse;     // rgb = diffuse color, w unused
    vec4 specular;    // rgb = specular color, w unused
    mat4 lightSpaceMatrix; // Used for shadow mapping.
};

layout(std140, binding = 1) uniform LightBlock {
    // counts.x = number of point lights, counts.y = spot, counts.z = directional.
    ivec4 counts;
    PointLightData pointLights[MAX_POINT_LIGHTS];
    SpotLightData spotLights[MAX_SPOT_LIGHTS];
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
uniform sampler2D  brdfLUT;           // 2D LUT for split–sum IBL

// ------------------------------------------------------------------------------------
// Shadow Mapping for Directional Lights
// ------------------------------------------------------------------------------------
// Each directional light has its own shadow map.
uniform sampler2D directionalShadowMaps[MAX_DIRECTIONAL_LIGHTS];
// (The light-space matrices come from the UBO: directionalLights[i].lightSpaceMatrix)
// Also, we tell the shader how many directional shadow maps to use.
uniform int numDirectionalShadows;

// ------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  fresnelSchlick(float cosTheta, vec3 F0);
vec3  fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3  getNormalFromMap();

// Updated shadow function now takes a sampler parameter.
float ShadowCalculation(vec4 fragPosLS, vec3 N, vec3 L, sampler2D shadowMap);

void mainPBR(out vec3 outColor);

// ------------------------------------------------------------------------------------
// Main Entry Point
// ------------------------------------------------------------------------------------
void main()
{
    vec3 color;
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
    vec3 V = normalize(cameraPos.xyz - FragPos);
    vec3 R = reflect(-V, N);
    vec3 F0 = mix(vec3(0.04), albedoColor, metallicValue);

    // 7) Direct Lighting Contribution.
    vec3 Lo = vec3(0.0);

    // ---- Point Lights (unchanged) ----
    for (int i = 0; i < counts.x; i++) {
        vec3 toLight = pointLights[i].position.xyz - FragPos;
        float dist = length(toLight);
        vec3 L = (dist > 0.0001) ? normalize(toLight) : vec3(0.0, 0.0, 1.0);
        float att = 1.0 / (
            pointLights[i].attenuation.x +
            pointLights[i].attenuation.y * dist +
            pointLights[i].attenuation.z * (dist * dist)
        );
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;
            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse = kD * albedoColor / PI;
            vec3 lightDiffuse = pointLights[i].diffuse.rgb * att;
            vec3 direct = (diffuse + specular) * lightDiffuse * NdotL;
            Lo += direct;
        }
    }

    // ---- Spot Lights (unchanged) ----
    for (int i = 0; i < counts.y; i++) {
        vec3 toLight = spotLights[i].position.xyz - FragPos;
        float dist = length(toLight);
        vec3 L = (dist > 0.0001) ? normalize(toLight) : vec3(0.0, 0.0, 1.0);
        float theta = dot(L, normalize(spotLights[i].direction.xyz));
        float innerCut = spotLights[i].cutoffs.x;
        float outerCut = spotLights[i].cutoffs.y;
        float intensity = clamp((theta - outerCut) / (innerCut - outerCut), 0.0, 1.0);
        float att = 1.0 / (
            spotLights[i].attenuation.x +
            spotLights[i].attenuation.y * dist +
            spotLights[i].attenuation.z * (dist * dist)
        );
        att *= intensity;
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;
            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse = kD * albedoColor / PI;
            vec3 lightDiffuse = spotLights[i].diffuse.rgb * att;
            vec3 direct = (diffuse + specular) * lightDiffuse * NdotL;
            Lo += direct;
        }
    }

    // ---- Directional Lights with Shadow Mapping ----
    for (int i = 0; i < counts.z; i++) {
        // For directional lights, L = -direction.
        vec3 L = normalize(-directionalLights[i].direction.xyz);
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;
            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse = kD * albedoColor / PI;
            vec3 lightDiffuse = directionalLights[i].diffuse.rgb;

            // Compute shadow factor for this directional light.
            vec4 fragPosLS = directionalLights[i].lightSpaceMatrix * vec4(FragPos, 1.0);
            float shadow = ShadowCalculation(fragPosLS, N, L, directionalShadowMaps[i]);
            vec3 direct = (diffuse + specular) * lightDiffuse * NdotL * (1.0 - shadow);
            Lo += direct;
        }
    }
    
    // ---- Image-Based Lighting (IBL) ----
    vec3 F_IBL = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughnessValue);
    vec3 kS = F_IBL;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * albedoColor;
    float NdotV = max(dot(N, V), 0.0);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughnessValue * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(NdotV, roughnessValue)).rg;
    vec3 specularIBL = prefilteredColor * (F_IBL * brdf.x + brdf.y);
    vec3 ambientIBL = (kD * diffuseIBL + specularIBL) * aoValue;
    
    // 9) Final Composition
    vec3 finalColor = ambientIBL + Lo + emissiveColor;
    outColor = finalColor;
}

// ------------------------------------------------------------------------------------
// Shadow Calculation Function (with PCF)
// ------------------------------------------------------------------------------------
float ShadowCalculation(vec4 fragPosLS, vec3 N, vec3 L, sampler2D shadowMap)
{
    // Perform perspective divide.
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    // Transform to [0,1] range.
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0)
        return 0.0;
    float bias = max(0.005 * (1.0 - dot(N, L)), 0.0005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    // 3x3 PCF.
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
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
    return normalize(TBN[2]);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) *
           pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
