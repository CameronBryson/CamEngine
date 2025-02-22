#version 460 core
precision mediump float;
precision mediump sampler2DShadow;
precision mediump samplerCubeShadow;
// ------------------------------------------------------------------------------------
// Outputs
// ------------------------------------------------------------------------------------
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


// ------------------------------------------------------------------------------------
// Vertex Inputs
// ------------------------------------------------------------------------------------
layout(location = 0) in vec2 TexCoord;
layout(location = 1) in vec3 FragPos;
layout(location = 2) in mat3 TBN;
layout (early_fragment_tests) in;

// ------------------------------------------------------------------------------------
// Constants & Limits
// ------------------------------------------------------------------------------------
const float PI = 3.14159265359;
const int   MAX_POINT_LIGHTS        = 10;
const int   MAX_SPOT_LIGHTS         = 10;
const int   MAX_DIRECTIONAL_LIGHTS  = 10;

// ------------------------------------------------------------------------------------
// Structures & Uniform Buffers
// ------------------------------------------------------------------------------------
layout(std140, binding = 0) uniform CameraBlock {
    mat4 view;
    mat4 projection;
    vec4 cameraPos;
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
    ivec4 counts; // counts.x = point lights, .y = spot, .z = directional
    PointLightData       pointLights[MAX_POINT_LIGHTS];
    SpotLightData        spotLights[MAX_SPOT_LIGHTS];
    DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

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
};

uniform Material material;
uniform bool   enableShadows;
uniform float  farPlane;
uniform float  bloomThreshold;

// IBL Maps
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D  brdfLUT;

// Shadow Maps
uniform sampler2DShadow  directionalShadowMap;
uniform sampler2DShadow  spotShadowMap;
uniform samplerCubeShadow pointShadowMap;

// ------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------
float  calculatePointShadow(vec3 lightPos);
float  calculateShadow(vec3 worldPos, vec3 normal, vec3 lightDir, mat4 lightSpaceMatrix, sampler2DShadow shadowMap);
vec3   getNormalFromMap();
vec3   perturbNormal(vec3 N, vec3 viewDir, float height);

// Cook-Torrance Microfacet (GGX, Smith, Schlick)
float  DistributionGGX(vec3 N, vec3 H, float roughness);
float  GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float  GeometrySchlickGGX(float NdotV, float roughness);
vec3   fresnelSchlick(float cosTheta, vec3 F0);
vec3   fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

// ------------------------------------------------------------------------------------
// Stepwise Functions for PBR
// ------------------------------------------------------------------------------------

// 1) Retrieve base albedo.
vec3 getBaseAlbedo()
{
    vec3 color = material.albedo.rgb;
    if (material.hasAlbedoMap) {
        color = texture(material.albedoMap, TexCoord).rgb;
    }
    return color;
}

// 2) Retrieve combined metallic & roughness values.
void getMetallicRoughness(out float metallicValue, out float roughnessValue)
{
    metallicValue  = material.metallic;
    roughnessValue = material.roughness;

    if (material.hasMetalRoughMap) {
        // Layout: R=?, G=Roughness, B=Metallic (depends on texture). 
        vec3 mr = texture(material.metalRoughMap, TexCoord).rgb;
        roughnessValue = mr.g;
        metallicValue  = mr.b;
    }
    else {
        if (material.hasMetallicMap) {
            metallicValue = texture(material.metallicMap, TexCoord).r;
        }
        if (material.hasRoughnessMap) {
            roughnessValue = texture(material.roughnessMap, TexCoord).r;
        }
    }
    roughnessValue = clamp(roughnessValue, 0.05, 1.0);
}

// 3) Retrieve ambient occlusion factor.
float getAOValue()
{
    if (material.hasAOMap) {
        return texture(material.AOMap, TexCoord).r;
    }
    return 1.0;
}

// 4) Retrieve emissive color.
vec3 getEmissiveColor()
{
    vec3 emissiveCol = material.emissiveColor * material.emissiveIntensity;
    if (material.hasEmissiveMap) {
        emissiveCol *= texture(material.emissiveMap, TexCoord).rgb;
    }
    return emissiveCol;
}

// 5) Direct lighting (Cook-Torrance) for each light type.
vec3 computeDirectLighting(vec3 N, vec3 V, vec3 F0, vec3 albedoColor, float metallicValue, float roughnessValue)
{
    vec3 Lo = vec3(0.0);

    // POINT LIGHTS
    for(int i = 0; i < counts.x; i++)
    {
        PointLightData pl = pointLights[i];
        vec3 L    = pl.position.xyz - FragPos;
        float dist = length(L);
        L          = normalize(L);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            // GGX/Smith/Schlick
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float denom   = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;

            vec3 kS = F; 
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse  = kD * albedoColor / PI;

            float attenuation = 1.0 / (pl.attenuation.x
                                     + pl.attenuation.y * dist
                                     + pl.attenuation.z * dist * dist);

            vec3 radiance     = pl.diffuse.rgb;
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL * attenuation;

            // Optional shadows
            if (enableShadows) {
                float shadowFactor = calculatePointShadow(pl.position.xyz);
                lightContrib *= shadowFactor;
            }
            Lo += lightContrib;
        }
    }

    // SPOT LIGHTS
    for(int i = 0; i < counts.y; i++)
    {
        SpotLightData sl = spotLights[i];
        vec3 L   = sl.position.xyz - FragPos;
        float dist = length(L);
        L         = normalize(L);

        float theta   = dot(L, normalize(-sl.direction.xyz));
        float epsilon = sl.cutoffs.x - sl.cutoffs.y;
        float intensity = clamp((theta - sl.cutoffs.y) / epsilon, 0.0, 1.0);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0 && intensity > 0.0)
        {
            vec3 H = normalize(V + L);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, L, roughnessValue);
            vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float denom   = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;

            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse = kD * albedoColor / PI;

            float attenuation = 1.0 / (sl.attenuation.x
                                     + sl.attenuation.y * dist
                                     + sl.attenuation.z * dist * dist);

            vec3 radiance     = sl.diffuse.rgb;
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL * attenuation * intensity;

            if (enableShadows) {
                float shadowFactor = calculateShadow(FragPos, N, L, sl.lightSpaceMatrix, spotShadowMap);
                lightContrib *= shadowFactor;
            }
            Lo += lightContrib;
        }
    }

    // DIRECTIONAL LIGHTS
    for(int i = 0; i < counts.z; i++)
    {
        DirectionalLightData dl = directionalLights[i];
        vec3 lightDir = normalize(-dl.direction.xyz);
        float NdotL   = max(dot(N, lightDir), 0.0);

        if (NdotL > 0.0)
        {
            vec3 H = normalize(V + lightDir);
            float D = DistributionGGX(N, H, roughnessValue);
            float G = GeometrySmith(N, V, lightDir, roughnessValue);
            vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            float denom   = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = (D * G * F) / denom;

            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);
            vec3 diffuse = kD * albedoColor / PI;

            vec3 radiance = dl.diffuse.rgb;
            vec3 lightContrib = (diffuse + specular) * radiance * NdotL;

            if (enableShadows) {
                float shadowFactor = calculateShadow(FragPos, N, lightDir, dl.lightSpaceMatrix, directionalShadowMap);
                lightContrib *= shadowFactor;
            }
            Lo += lightContrib;
        }
    }

    return Lo;
}

// 6) Image-Based Lighting (IBL) with diffuse irradiance & specular pre-filtering.
vec3 computeIBL(vec3 N, vec3 R, vec3 V, vec3 F0, vec3 albedoColor, float metallicValue, float roughnessValue, float aoValue)
{
    const float MAX_REFLECTION_LOD = 4.0;
    float NdotV = max(dot(N, V), 0.0);

    // Fresnel factor for IBL
    vec3 F_IBL = fresnelSchlickRoughness(NdotV, F0, roughnessValue);

    // Reflection ratio (spec) & (diffuse)
    vec3 kS = F_IBL;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);

    // Diffuse IBL
    vec3 irradiance  = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL  = irradiance * albedoColor;

    // Specular IBL
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughnessValue * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(NdotV, roughnessValue)).rg;
    vec3 specularIBL = prefilteredColor * (F_IBL * brdf.x + brdf.y);

    // Combine
    vec3 ambientIBL = (kD * diffuseIBL + specularIBL) * aoValue;
    return ambientIBL * 0.1f;
    return ambientIBL;
}

// ------------------------------------------------------------------------------------
// Main Flow
// ------------------------------------------------------------------------------------
void mainPBR(out vec3 outColor)
{
    // Prepare albedo
    vec3 albedoColor = getBaseAlbedo();

    // Prepare normal
    vec3 N = getNormalFromMap();
    if (material.hasDisplacementMap && material.displacementScale > 0.0)
    {
        float height = texture(material.displacementMap, TexCoord).r;
        vec3 viewDir = normalize(cameraPos.xyz - FragPos);
        N = perturbNormal(N, viewDir, height * material.displacementScale);
    }

    // Metallic & roughness
    float metallicValue;
    float roughnessValue;
    getMetallicRoughness(metallicValue, roughnessValue);

    // AO
    float aoValue = getAOValue();

    // Emissive
    vec3 emissiveColor = getEmissiveColor();

    // View & reflection
    vec3 V  = normalize(cameraPos.xyz - FragPos);
    vec3 R  = reflect(-V, N);

    // Fresnel base reflectivity
    vec3 F0 = mix(vec3(0.04), albedoColor, metallicValue);

    // Direct lighting (Cook-Torrance)
    vec3 Lo = computeDirectLighting(N, V, F0, albedoColor, metallicValue, roughnessValue);

    // IBL lighting
    vec3 ambientIBL = computeIBL(N, R, V, F0, albedoColor, metallicValue, roughnessValue, aoValue);

    // Combine all
    outColor = ambientIBL + Lo + emissiveColor;
}

// ------------------------------------------------------------------------------------
// Final Fragment Entry Point
// ------------------------------------------------------------------------------------
void main()
{
    // Combine final alpha
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

    // Early discard
    if (alpha < 0.1) {
        discard;
    }

    // Calculate PBR color
    vec3 color;
    mainPBR(color);
    FragColor = vec4(color, alpha);

    // Extract brightness for bloom
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    BrightColor = (brightness > bloomThreshold) ? vec4(color, 1.0) : vec4(0.0);
}

// ------------------------------------------------------------------------------------
// Shadows (unchanged)
// ------------------------------------------------------------------------------------
float calculatePointShadow(vec3 lightPos)
{
    // Get vector from fragment to light
    vec3 fragToLight = FragPos - lightPos;
    
    // Current depth
    float currentDepth = length(fragToLight) / farPlane;
    
    // Bias based on depth
    float bias = 0.05 * currentDepth;
    
    // Hardware PCF sampling
    float shadow = 0.0;
    float samples = 4.0;
    float offset = 0.1;
    
    // Sample multiple times for soft shadows
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                vec3 sampleVec = fragToLight + vec3(x, y, z);
                // Hardware comparison happens here - returns filtered 0 to 1
                shadow += texture(pointShadowMap, vec4(normalize(sampleVec), currentDepth - bias));
            }
        }
    }
    
    shadow /= (samples * samples * samples);
    return shadow;
}

float calculateShadow(vec3 worldPos, vec3 normal, vec3 lightDir, mat4 lightSpaceMatrix, sampler2DShadow shadowMap)
{
    // Transform to light space
    vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get depth bias
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    // Early out if beyond far plane
    if(projCoords.z > 1.0)
        return 1.0;
        
    // PCF sampling
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            // Hardware PCF - returns filtered 0 to 1
            shadow += texture(shadowMap, 
                vec3(projCoords.xy + vec2(x, y) * texelSize, 
                     projCoords.z - bias));
        }
    }
    
    shadow /= 9.0;
    return shadow;
}

// ------------------------------------------------------------------------------------
// Normal & Displacement
// ------------------------------------------------------------------------------------
vec3 getNormalFromMap()
{
    if(material.hasNormalMap) {
        vec3 tNormal = texture(material.normalMap, TexCoord).rgb;
        tNormal = tNormal * 2.0 - 1.0;
        return normalize(TBN * tNormal);
    }
    return normalize(TBN[2]);
}

vec3 perturbNormal(vec3 N, vec3 viewDir, float height)
{
    vec2 texSize   = 1.0 / vec2(textureSize(material.displacementMap, 0));
    float hL       = texture(material.displacementMap, TexCoord - vec2(texSize.x, 0.0)).r;
    float hR       = texture(material.displacementMap, TexCoord + vec2(texSize.x, 0.0)).r;
    float hD       = texture(material.displacementMap, TexCoord - vec2(0.0, texSize.y)).r;
    float hU       = texture(material.displacementMap, TexCoord + vec2(0.0, texSize.y)).r;

    vec3 normal    = normalize(N);
    vec3 tangent   = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
    vec3 bitangent = normalize(cross(normal, tangent));

    float ddx      = (hR - hL);
    float ddy      = (hU - hD);
    float scale    = 0.04;

    vec3 perturbed = normal + tangent * ddx * scale + bitangent * ddy * scale;
    return normalize(mix(normal, perturbed, height));
}

// ------------------------------------------------------------------------------------
// Cook-Torrance Microfacet BRDF (GGX, Smith, Schlick)
// ------------------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a     = roughness * roughness;
    float a2    = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2= NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom; // GGX
    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Smith's Schlick-GGX
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
    // Schlick's approximation for Fresnel
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    // Variation that modifies Fresnel by roughness
    return F0 + (max(vec3(1.0 - roughness), F0) - F0)
           * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
