#version 450 core

// ----------------------------
// Output
// ----------------------------
out vec4 FragColor;

// ----------------------------
// Inputs from Vertex Shader
// ----------------------------
in vec2 TexCoord;
in vec3 FragPos;    // Expected to be in world space.
in mat3 TBN;        // Tangent, Bitangent, Normal (for normal mapping)

// ----------------------------
// Constants
// ----------------------------
const float PI = 3.14159265359;
const int MAX_POINT_LIGHTS = 25;
const int MAX_SPOT_LIGHTS = 25;
const int MAX_DIRECTIONAL_LIGHTS = 25;

// ----------------------------
// Uniform Blocks (std140 layout)
// ----------------------------

// Camera data block (binding = 0)
// Make sure your engine binds this buffer with CameraData.
layout(std140, binding = 0) uniform CameraBlock {
    mat4 view;
    mat4 projection;
    vec4 cameraPos;  // xyz = camera position; w is padding
};

// Light data block (binding = 1)
// The light structures below must match exactly what your engine uploads.
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
    vec4 cutoffs;     // x = inner cutoff, y = outer cutoff, z,w unused
};

struct DirectionalLightData {
    vec4 direction;   // xyz = direction, w unused
    vec4 ambient;     // rgb = ambient, w unused
    vec4 diffuse;     // rgb = diffuse, w unused
    vec4 specular;    // rgb = specular, w unused
};

layout(std140, binding = 1) uniform LightBlock {
    // counts.x = number of point lights
    // counts.y = number of spot lights
    // counts.z = number of directional lights
    // counts.w = padding
    ivec4 counts;
    PointLightData pointLights[MAX_POINT_LIGHTS];
    SpotLightData spotLights[MAX_SPOT_LIGHTS];
    DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

// ----------------------------
// Material Uniform (not in a UBO)
// ----------------------------
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

// ----------------------------
// IBL Samplers
// ----------------------------
uniform samplerCube irradianceMap;   // Diffuse irradiance
uniform samplerCube prefilterMap;    // Prefiltered specular environment
uniform sampler2D  brdfLUT;          // 2D LUT for split–sum IBL

// ----------------------------
// Function Prototypes
// ----------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 getNormalFromMap();
void mainPBR(out vec3 outColor);

// ----------------------------
// Main Entry Point
// ----------------------------
void main()
{
    vec3 color;
    mainPBR(color);

    // Apply Reinhard tone mapping.
    color = color / (color + vec3(1.0));

    // Gamma correction (linear to sRGB)
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}

// ----------------------------
// Core PBR Implementation
// ----------------------------
void mainPBR(out vec3 outColor)
{
    // 1) Base Albedo: sample from texture if available and convert sRGB -> linear.
    vec3 albedoColor = material.albedo.rgb;
    if (material.hasAlbedoMap) {
        albedoColor = texture(material.albedoMap, TexCoord).rgb;
        albedoColor = pow(albedoColor, vec3(2.2));
    }

    // 2) Normal Mapping: if a normal map exists, sample and transform with TBN.
    vec3 N = getNormalFromMap();

    // 3) Metallic & Roughness: sample from a combined metal–rough map or separate maps,
    //    otherwise use constant values.
    float metallicValue = material.metallic;
    float roughnessValue = material.roughness;
    if (material.hasMetalRoughMap) {
        vec3 mrSample = texture(material.metalRoughMap, TexCoord).rgb;
        // Assumes: green channel stores roughness, blue stores metallic.
        roughnessValue = mrSample.g;
        metallicValue = mrSample.b;
    } else {
        if (material.hasMetallicMap)
            metallicValue = texture(material.metallicMap, TexCoord).r;
        if (material.hasRoughnessMap)
            roughnessValue = texture(material.roughnessMap, TexCoord).r;
    }
    // Clamp roughness to avoid zero?roughness (which would cause singular highlights)
    roughnessValue = clamp(roughnessValue, 0.05, 1.0);

    // 4) Ambient Occlusion
    float aoValue = material.AO;
    if (material.hasAOMap)
        aoValue = texture(material.AOMap, TexCoord).r;

    // 5) Emissive Color
    vec3 emissiveColor = vec3(0.0);
    if (material.hasEmissiveMap)
        emissiveColor = texture(material.emissiveMap, TexCoord).rgb;

    // 6) Compute View and Reflection Vectors:
    // Use the camera position from the uniform block (in world space)
    vec3 V = normalize(cameraPos.xyz - FragPos);
    // Reflect the inverted view vector about the normal.
    vec3 R = reflect(-V, N);
    // F0: base reflectivity; non-metal surfaces use ~0.04.
    vec3 F0 = mix(vec3(0.04), albedoColor, metallicValue);

    // 7) Direct Lighting (Point Lights Only)
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < counts.x; i++) {
        // Compute light vector from fragment position to light.
        vec3 L = pointLights[i].position.xyz - FragPos;
        float distance = length(L);
        L = normalize(L);
        
        // Compute attenuation using constant, linear, and quadratic terms.
        float attenuation = 1.0 / (
            pointLights[i].attenuation.x +
            pointLights[i].attenuation.y * distance +
            pointLights[i].attenuation.z * (distance * distance)
        );
        // Radiance is the diffuse color of the light scaled by attenuation.
        vec3 radiance = pointLights[i].diffuse.rgb * attenuation;

        // Compute halfway vector.
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            // Microfacet distribution, geometry, and Fresnel terms.
            float NDF = DistributionGGX(N, H, roughnessValue);
            float G   = GeometrySmith(N, V, L, roughnessValue);
            vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (NDF * G * F) / denom;

            // Energy conservation: diffuse (kD) = 1 - Fresnel (kS)
            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse = kD * albedoColor / PI;

            // Optional ambient contribution from the light (using its ambient color)
            vec3 ambientLight = pointLights[i].ambient.rgb * attenuation;
            Lo += (diffuse + specular) * radiance * NdotL + ambientLight;
        }
    }

    // 8) Image-Based Lighting (IBL)
    // Compute Fresnel using the roughness variant.
    vec3 F_IBL = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughnessValue);
    vec3 kS = F_IBL;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
    // Sample the irradiance cubemap for diffuse IBL.
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * albedoColor;
    // Compute specular IBL using the prefiltered environment and BRDF LUT.
    float NdotV = max(dot(N, V), 0.0);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughnessValue * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(NdotV, roughnessValue)).rg;
    vec3 specularIBL = prefilteredColor * (F_IBL * brdf.x + brdf.y);
    // Combine diffuse and specular IBL, modulated by ambient occlusion.
    vec3 ambient = (kD * diffuseIBL + specularIBL) * aoValue;

    // 9) Final Combination: add direct lighting, IBL, and emissive contribution.
    vec3 color = ambient + Lo + emissiveColor;
    outColor = color;
}

// ----------------------------
// Helper Functions
// ----------------------------

// Normal Mapping: sample the normal map and transform via TBN.
// If no normal map is provided, use the interpolated normal (third column of TBN).
vec3 getNormalFromMap() {
    if (material.hasNormalMap) {
        vec3 tangentNormal = texture(material.normalMap, TexCoord).rgb * 2.0 - 1.0;
        return normalize(TBN * tangentNormal);
    }
    return normalize(TBN[2]);
}

// GGX Normal Distribution Function (NDF)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a     = roughness * roughness;
    float a2    = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}

// Schlick–GGX Geometry function for one direction.
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

// Smith's method to combine geometry terms for both view and light.
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    return ggx1 * ggx2;
}

// Fresnel Schlick approximation.
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fresnel Schlick variant that incorporates roughness.
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    // Push F0 toward 1.0 as roughness increases.
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) *
           pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
