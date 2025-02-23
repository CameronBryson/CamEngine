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
// Inputs
// ------------------------------------------------------------------------------------
in vec2 TexCoord;

// ------------------------------------------------------------------------------------
// Constants & Limits
// ------------------------------------------------------------------------------------
const float PI = 3.14159265359;
const int   MAX_POINT_LIGHTS        = 10;
const int   MAX_SPOT_LIGHTS         = 10;
const int   MAX_DIRECTIONAL_LIGHTS  = 10;

// ------------------------------------------------------------------------------------
// Camera & Light Data
// ------------------------------------------------------------------------------------
layout(std140, binding = 0) uniform CameraBlock {
    mat4 view;
    mat4 projection;
    vec4 cameraPos; // .xyz is camera position
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
    PointLightData       pointLights[MAX_POINT_LIGHTS];
    SpotLightData        spotLights[MAX_SPOT_LIGHTS];
    DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

// ------------------------------------------------------------------------------------
// G-Buffer Textures
// ------------------------------------------------------------------------------------
uniform sampler2D gAlbedoAO;       // RGB: Albedo, A: AO
uniform sampler2D gNormalMetallic; // RGB: Normal, A: Metallic
uniform sampler2D gRoughEmissive;  // R: Roughness, GBA: Emissive
uniform sampler2D gDepth;          // Depth (for position reconstruction)

// ------------------------------------------------------------------------------------
// Environment & Shadows
// ------------------------------------------------------------------------------------
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;

uniform sampler2DShadow  directionalShadowMap;
uniform sampler2DShadow  spotShadowMap;
uniform samplerCubeShadow pointShadowMap;

// ------------------------------------------------------------------------------------
// Misc Uniforms
// ------------------------------------------------------------------------------------
uniform bool   enableShadows;
uniform float  farPlane;
uniform float  bloomThreshold;
uniform sampler2D ssaoTexture;
uniform bool ssaoEnabled;
// ------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------
float calculatePointShadow(vec3 FragPos, vec3 lightPos);
float calculateShadow(vec3 FragPos, vec3 normal, vec3 lightDir, mat4 lightSpaceMatrix, sampler2DShadow shadowMap);

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3  fresnelSchlick(float cosTheta, vec3 F0);
vec3  fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

// ------------------------------------------------------------------------------------
// Position Reconstruction from Depth
// ------------------------------------------------------------------------------------
vec3 reconstructWorldPos(float depth, vec2 texCoords) 
{
    // Early exit if depth is 1.0 (far plane/background)
    if (depth >= 1.0) return vec3(0.0);
    
    float z = depth * 2.0 - 1.0;
    vec4 clipSpacePos = vec4(texCoords * 2.0 - 1.0, z, 1.0);
    
    vec4 viewSpacePos = inverse(projection) * clipSpacePos;
    viewSpacePos /= viewSpacePos.w;
    
    vec4 worldSpacePos = inverse(view) * viewSpacePos;
    return worldSpacePos.xyz;
}


// ------------------------------------------------------------------------------------
// Direct Lighting (Cook-Torrance) Adaptation
// ------------------------------------------------------------------------------------
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

// ------------------------------------------------------------------------------------
// Image-Based Lighting (IBL)
// ------------------------------------------------------------------------------------
vec3 computeIBL(vec3 N, vec3 V, vec3 R, vec3 F0, vec3 albedo, float metallic, float roughness, float ao)
{
    float NdotV = max(dot(N, V), 0.0);
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
    
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    
    // Get SSAO value and combine with material AO
    float ssaoValue = ssaoEnabled ? texture(ssaoTexture, TexCoord).r : 1.0;
    float finalAO = min(ao, ssaoValue); // Use the more occluded value
    
    // Diffuse IBL
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    
    // Specular IBL
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    // Apply combined AO to both diffuse and specular IBL
    vec3 ambient = (kD * diffuse + specular) * finalAO;
    return ambient * 0.15; // Match forward renderer IBL intensity
}



// ------------------------------------------------------------------------------------
// Main Fragment
// ------------------------------------------------------------------------------------
void main()
{
    // 1) Read from G-Buffer
    vec4 albedoAO       = texture(gAlbedoAO, TexCoord);
    vec4 normalMetallic = texture(gNormalMetallic, TexCoord);
    vec4 roughEmissive  = texture(gRoughEmissive, TexCoord);
    float fragDepth     = texture(gDepth, TexCoord).r;
    
    // 2) Unpack data from G-Buffer
    vec3 albedo = albedoAO.rgb;
    float ao = albedoAO.a;
    vec3 N = normalize(normalMetallic.rgb * 2.0 - 1.0);
    float metallic = normalMetallic.a;
    float roughness = roughEmissive.r;
    vec3 emissive = roughEmissive.gba;
    
    // Get SSAO value early
    float ssaoValue = ssaoEnabled ? texture(ssaoTexture, TexCoord).r : 1.0;
    // Combine material AO with SSAO
    ao = min(ao, ssaoValue);
    
    // Early exit if background/sky
    if (fragDepth >= 1.0) {
        FragColor = vec4(0.0);
        BrightColor = vec4(0.0);
        return;
    }
    
    // 3) Reconstruct position
    vec3 FragPos = reconstructWorldPos(fragDepth, TexCoord);
    
    // 4) Compute view vector and reflection vector
    vec3 V = normalize(cameraPos.xyz - FragPos);
    vec3 R = reflect(-V, N);
    
    // 5) Base reflectivity
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    // 6) Compute lighting
    vec3 direct = computeDirectLighting(FragPos, N, V, F0, albedo, metallic, roughness);
    vec3 ibl = computeIBL(N, V, R, F0, albedo, metallic, roughness, ao);
    
    // 7) Combine all lighting contributions
    vec3 color = direct + ibl + emissive;
    
    // 8) Add ambient term with SSAO
    color += albedo * 0.01 * ao; // Small ambient term affected by SSAO
    
    // Debug views
    // FragColor = vec4(vec3(ssaoValue), 1.0); return;    // Debug SSAO
    // FragColor = vec4(vec3(ao), 1.0); return;           // Debug combined AO
    // FragColor = vec4(direct, 1.0); return;             // Debug direct lighting
    // FragColor = vec4(ibl, 1.0); return;                // Debug IBL lighting
    // FragColor = vec4(emissive, 1.0); return;           // Debug emissive
    
    // 9) Output final color
    FragColor = vec4(color, 1.0);
    
    // 10) Compute bloom contribution
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    BrightColor = (brightness > bloomThreshold) ? vec4(color, 1.0) : vec4(0.0);
}



// ------------------------------------------------------------------------------------
// Shadow Functions and BRDF Helpers
// ------------------------------------------------------------------------------------
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
