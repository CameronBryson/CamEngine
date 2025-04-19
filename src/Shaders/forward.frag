#version 460 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;
in vec4 ClipPos;
in vec4 PrevClipPos;

const float PI = 3.14159265359;
const int MAX_POINT_LIGHTS = 10;
const int MAX_SPOT_LIGHTS = 10;
const int MAX_DIRECTIONAL_LIGHTS = 10;

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

struct PointLightData {
    vec4 position;   
    vec4 ambient;    
    vec4 diffuse;    
    vec4 specular;   
    vec4 attenuation;
    mat4 shadowMatrices[6];
};

struct SpotLightData {
    vec4 position;   
    vec4 direction;  
    vec4 ambient;    
    vec4 diffuse;    
    vec4 specular;   
    vec4 attenuation;
    vec4 cutoffs;    
    mat4 lightSpaceMatrix;
};

struct DirectionalLightData {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    mat4 lightSpaceMatrix;
};

layout(std140, binding = 1) uniform LightBlock {
    ivec4 counts;
    PointLightData pointLights[MAX_POINT_LIGHTS];
    SpotLightData spotLights[MAX_SPOT_LIGHTS];
    DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

// PBR Material properties
struct Material {
    bool hasAlbedoMap;
    bool hasNormalMap;
    bool hasMetallicMap;
    bool hasRoughnessMap;
    bool hasMetalRoughMap;
    bool hasAOMap;
    bool hasEmissiveMap;
    bool hasOpacityMap;

    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D metalRoughMap;
    sampler2D AOMap;
    sampler2D emissiveMap;
    sampler2D opacityMap;

    vec4  albedo;
    float metallic;
    float roughness;
    float opacity;
    vec3  emissiveColor;
    float emissiveIntensity;
    float reflectivity;
};

uniform Material material;
// Environment maps for PBR image-based lighting
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Shadow maps
uniform sampler2DShadow directionalShadowMap;
uniform sampler2DShadow spotShadowMap;
uniform samplerCubeShadow pointShadowMap;



uniform bool enableShadows;
uniform float farPlane;

// Add SSAO texture and enable flag to match deferred shader
uniform sampler2D ssaoTexture;
uniform bool ssaoEnabled;

// Function declarations
float calculatePointShadow(vec3 FragPos, vec3 lightPos);
float calculateShadow(vec3 worldPos, vec3 normal, vec3 lightDir, mat4 lightSpaceMatrix, sampler2DShadow shadowMap);

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

// Get normal from normal map or calculate from TBN
vec3 getNormalFromMap() {
    if(material.hasNormalMap) {
        // Sample normal from texture
        vec3 tangentNormal = texture(material.normalMap, TexCoord).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0;
        
        vec3 worldNormal = TBN * tangentNormal;
        return normalize(worldNormal);
    }
    
    return normalize(TBN[2]); // Normal is the third vector in TBN matrix
}

vec3 computeDirectLighting(vec3 FragPos, vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness)
{
    vec3 Lo = vec3(0.0);
    
    // --- Point Lights Loop ---
    for(int i = 0; i < counts.x; i++)
    {
        PointLightData pl = pointLights[i];
        vec3 L = pl.position.xyz - FragPos;
        float dist = length(L);
        L = normalize(L);
        
        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;
            
            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
            vec3 diffuse = kD * albedo / PI;
            
            float attenuation = 1.0 / (pl.attenuation.x + pl.attenuation.y * dist + pl.attenuation.z * dist * dist);
            vec3 radiance = pl.diffuse.rgb;
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL * attenuation;
            
            if(enableShadows)
            {
                float shadow = calculatePointShadow(FragPos, pl.position.xyz);
                lightContrib *= shadow;
            }
            Lo += lightContrib;
        }
    }
    
    // --- Spot Lights Loop ---
    for(int i = 0; i < counts.y; i++)
    {
        SpotLightData sl = spotLights[i];
        vec3 L = sl.position.xyz - FragPos;
        float dist = length(L);
        L = normalize(L);
        float theta = dot(L, normalize(-sl.direction.xyz));
        float epsilon = sl.cutoffs.x - sl.cutoffs.y;
        float intensity = clamp((theta - sl.cutoffs.y) / epsilon, 0.0, 1.0);
        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0 && intensity > 0.0)
        {
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;
            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
            vec3 diffuse = kD * albedo / PI;
            float attenuation = 1.0 / (sl.attenuation.x + sl.attenuation.y * dist + sl.attenuation.z * dist * dist);
            vec3 radiance = sl.diffuse.rgb;
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL * attenuation * intensity;
            if(enableShadows)
            {
                float shadow = calculateShadow(FragPos, N, L, sl.lightSpaceMatrix, spotShadowMap);
                lightContrib *= shadow;
            }
            Lo += lightContrib;
        }
    }
    
    // --- Directional Lights Loop ---
    for(int i = 0; i < counts.z; i++)
    {
        DirectionalLightData dl = directionalLights[i];
        vec3 lightDir = normalize(-dl.direction.xyz);
        float NdotL = max(dot(N, lightDir), 0.0);
        if(NdotL > 0.0)
        {
            vec3 H = normalize(V + lightDir);
            float D = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, lightDir, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;
            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
            vec3 diffuse = kD * albedo / PI;
            vec3 radiance = dl.diffuse.rgb;
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL;
            if(enableShadows)
            {
                float shadow = calculateShadow(FragPos, N, lightDir, dl.lightSpaceMatrix, directionalShadowMap);
                lightContrib *= shadow;
            }
            Lo += lightContrib;
        }
    }
    
    return Lo;
}

vec3 computeIBL(vec3 N, vec3 V, vec3 R, vec3 F0, vec3 albedo, float metallic, float roughness, float ao)
{
    float NdotV = max(dot(N, V), 0.0);
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
    
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    
    // Get SSAO value and combine with material AO - UPDATED to match deferred.frag
    float ssaoValue = ssaoEnabled ? texture(ssaoTexture, TexCoord).r : 1.0;
    // Use min instead of averaging - this matches deferred shader
    float finalAO = min(ao, ssaoValue);
    
    // Diffuse IBL
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    
    // Specular IBL
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    // Apply AO to both diffuse and specular IBL
    vec3 ambient = (kD * diffuse + specular) * finalAO;
    return ambient * 0.7; // Keep the 0.7 multiplier as it's in deferred.frag
}

void main() {
    // Sample material properties
    vec4 albedoSample = material.hasAlbedoMap ? texture(material.albedoMap, TexCoord) : material.albedo;
    vec3 albedo = albedoSample.rgb;
    
    float alpha = material.opacity;
    if (material.hasOpacityMap) {
        alpha *= texture(material.opacityMap, TexCoord).r;
    } else if (material.hasAlbedoMap) {
        alpha *= albedoSample.a;
    }
    
    // Skip very transparent fragments
    if (alpha < 0.01) {
        discard;
    }
    
    // Sample metallic, roughness, normal, and AO
    float metallic = material.metallic;
    if (material.hasMetalRoughMap) {
        metallic = texture(material.metalRoughMap, TexCoord).b;
    } else if (material.hasMetallicMap) {
        metallic = texture(material.metallicMap, TexCoord).r;
    }
    
    float roughness = material.roughness;
    if (material.hasMetalRoughMap) {
        roughness = texture(material.metalRoughMap, TexCoord).g;
    } else if (material.hasRoughnessMap) {
        roughness = texture(material.roughnessMap, TexCoord).r;
    }
    roughness = clamp(roughness, 0.05, 1.0);
    
    float ao = material.hasAOMap ? texture(material.AOMap, TexCoord).r : 1.0;
    
    // Get SSAO value early - NEW addition to match deferred.frag
    float ssaoValue = ssaoEnabled ? texture(ssaoTexture, TexCoord).r : 1.0;
    // Combine material AO with SSAO using min instead of average
    ao = min(ao, ssaoValue);
    
    vec3 emissive = material.emissiveColor * material.emissiveIntensity;
    if (material.hasEmissiveMap) {
        emissive *= texture(material.emissiveMap, TexCoord).rgb;
    }
    
    // Get normal and calculate view vector and reflection vector
    vec3 N = getNormalFromMap();
    vec3 V = normalize(cameraPos.xyz - FragPos);
    vec3 R = reflect(-V, N);
    
    // Calculate base reflectivity (F0)
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    // Compute all lighting components
    vec3 direct = computeDirectLighting(FragPos, N, V, F0, albedo, metallic, roughness);
    vec3 ibl = computeIBL(N, V, R, F0, albedo, metallic, roughness, ao);
    
    // Combine all lighting contributions
    vec3 color = direct + ibl + emissive;
    
    // Add ambient term with AO - now using ao directly which includes SSAO
    color += albedo * 0.01 * ao; // Small ambient term affected by AO
    
    // Output final color with alpha
    FragColor = vec4(color, alpha);
}

float calculatePointShadow(vec3 FragPos, vec3 lightPos)
{
    // Calculate vector from fragment to light
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight) / farPlane;
    float bias = 0.05 * currentDepth;
    float shadow = 0.0;
    float samples = 4.0;
    float offset = 0.1;
    
    // Perform a 3D PCF (hardware PCF on shadows)
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                vec3 sampleVec = fragToLight + vec3(x, y, z);
                shadow += texture(pointShadowMap, vec4(normalize(sampleVec), currentDepth - bias));
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

float calculateShadow(vec3 worldPos, vec3 normal, vec3 lightDir, mat4 lightSpaceMatrix, sampler2DShadow shadowMap)
{
    vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    if(projCoords.z > 1.0)
        return 1.0;
        
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            shadow += texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z - bias));
        }
    }
    shadow /= 9.0;
    return shadow;
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
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
