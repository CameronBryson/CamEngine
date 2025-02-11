#version 450 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;

// ------------------------------------------------------------------
// Material & Light Structures
// ------------------------------------------------------------------
struct Material
{
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

    vec4  albedo;
    float metallic;
    float roughness;
    float AO;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

// ------------------------------------------------------------------
// Uniforms
// ------------------------------------------------------------------
uniform int numPointLights;
uniform PointLight pointLights[50];

uniform Material material;
uniform vec3 viewPos;

// Environment maps for IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// ------------------------------------------------------------------
// Constants & Prototypes
// ------------------------------------------------------------------
const float PI = 3.14159265359;

float  DistributionGGX(vec3 N, vec3 H, float roughness);
float  GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3   fresnelSchlick(float cosTheta, vec3 F0);
vec3   fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3   getNormalFromMap();
void   mainPBR(out vec3 outColor);

// ------------------------------------------------------------------
// Main
// ------------------------------------------------------------------
void main()
{
    vec3 color;
    mainPBR(color);

    // Optional Tone Mapping (Reinhard)
    color = color / (color + vec3(1.0));

    // Final Gamma Correction (linear -> sRGB)
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}

// ------------------------------------------------------------------
// Core PBR Implementation
// ------------------------------------------------------------------
void mainPBR(out vec3 outColor)
{
    // 1) Albedo (Gamma-correct if from a texture)
    vec3 albedoColor = material.albedo.rgb;
    if (material.hasAlbedoMap)
    {
        albedoColor = texture(material.albedoMap, TexCoord).rgb;
        albedoColor = pow(albedoColor, vec3(2.2)); // sRGB -> linear
    }

    // 2) Normal Mapping
    vec3 N = getNormalFromMap();

    // 3) Metallic & Roughness
    float metallicValue  = material.metallic;
    float roughnessValue = material.roughness;

    if (material.hasMetalRoughMap)
    {
        // Adjust indexing if your .dds or .png stores them differently
        // Example: .g = roughness, .b = metallic
        vec3 mrSample = texture(material.metalRoughMap, TexCoord).rgb;
        roughnessValue = mrSample.g;
        metallicValue  = mrSample.b;
    }
    else
    {
        if (material.hasMetallicMap)
            metallicValue = texture(material.metallicMap, TexCoord).r;
        if (material.hasRoughnessMap)
            roughnessValue = texture(material.roughnessMap, TexCoord).r;
    }
    roughnessValue = clamp(roughnessValue, 0.05, 1.0);

    // 4) Ambient Occlusion
    float aoValue = material.AO;
    if (material.hasAOMap)
        aoValue = texture(material.AOMap, TexCoord).r;

    // 5) Emissive
    vec3 emissiveColor = vec3(0.0);
    if (material.hasEmissiveMap)
        emissiveColor = texture(material.emissiveMap, TexCoord).rgb;

    // 6) V, R, and base F0
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-V, N);
    vec3 F0 = mix(vec3(0.04), albedoColor, metallicValue);

    // 7) Direct Lighting (Point Lights)
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numPointLights; i++)
    {
        vec3 L = pointLights[i].position - FragPos;
        float distance = length(L);
        L = normalize(L);

        float attenuation = 1.0 / (distance * distance);
        // If you want to incorporate linear/quadratic terms:
        // attenuation = 1.0 / (pointLights[i].constant +
        //                      pointLights[i].linear * distance +
        //                      pointLights[i].quadratic * (distance * distance));

        vec3 radiance = pointLights[i].diffuse * attenuation;

        // Cook-Torrance
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            float NDF = DistributionGGX(N, H, roughnessValue);
            float G   = GeometrySmith(N, V, L, roughnessValue);
            vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float denom    = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3  specular = (NDF * G * F) / denom;

            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse = kD * albedoColor / PI;

            Lo += (diffuse + specular) * radiance * NdotL;
        }
    }

    // 8) Image-Based Lighting
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughnessValue);
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);

    // Diffuse IBL
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * albedoColor;

    // Specular IBL
    float NdotV = max(dot(N, V), 0.0);
    const float MAX_REFLECTION_LOD = 4.0; // or 5.0 if your prefilter is built that way
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughnessValue * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(NdotV, roughnessValue)).rg;
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuseIBL + specularIBL) * aoValue;

    // 9) Final Combine
    vec3 color = ambient + Lo + emissiveColor;

    outColor = color;
}

// ------------------------------------------------------------------
// Normal Mapping
// ------------------------------------------------------------------
vec3 getNormalFromMap()
{
    if (material.hasNormalMap)
    {
        vec3 tangentNormal = texture(material.normalMap, TexCoord).xyz * 2.0 - 1.0;
        return normalize(TBN * tangentNormal);
    }
    return normalize(TBN[2]);
}

// ------------------------------------------------------------------
// Microfacet Functions
// ------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a     = roughness * roughness;
    float a2    = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2= NdotH * NdotH;

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
    // Let high roughness push F0 closer to 1.0 for more diffuse reflection
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * 
           pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
