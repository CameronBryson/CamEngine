#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;

// --------------------------------------------------
// Material structure
// --------------------------------------------------
struct Material
{
    // Basic texture booleans
    bool hasAlbedoMap;
    bool hasNormalMap;

    // Separate textures
    bool hasMetallicMap;
    bool hasRoughnessMap;

    // Combined metal-rough
    bool hasMetalRoughMap;

    bool hasAOMap;
    bool hasEmissiveMap;

    // Samplers
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D metalRoughMap; // Combined
    sampler2D AOMap;
    sampler2D emissiveMap;

    // Fallback values
    vec4 albedo;
    float metallic;
    float roughness;
    float AO;
};

// --------------------------------------------------
// Point Light (for simplicity; ignore directional in example)
// --------------------------------------------------
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform int numPointLights;
uniform PointLight pointLights[50];

uniform Material material;
uniform vec3 viewPos;

uniform samplerCube environmentMap;

// (If you still want directional lights, keep them. We omit here for brevity.)

// --------------------------------------------------
// Constants & Helper Functions
// --------------------------------------------------
const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  fresnelSchlick(float cosTheta, vec3 F0);

// --------------------------------------------------
// Main
// --------------------------------------------------
void main()
{
    // 1) Base color (if sRGB loaded, no pow. If not, do pow(...,2.2))
    vec3 albedoColor = material.albedo.rgb;
    if (material.hasAlbedoMap)
    {
        // If your engine loads albedoMap as sRGB, just read it:
        albedoColor = texture(material.albedoMap, TexCoord).rgb;
    }

    // 2) Normal
    vec3 N;
    if (material.hasNormalMap)
    {
        vec3 tangentNormal = texture(material.normalMap, TexCoord).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0;
        N = normalize(TBN * tangentNormal);
    }
    else
    {
        // fallback, if you store geometry normal or TBN’s z-axis
        N = normalize(TBN[2]);
    }

    // 3) Metallic & Roughness
    float metallicValue  = material.metallic;
    float roughnessValue = material.roughness;

    if (material.hasMetalRoughMap)
    {
        // If combined texture: G=Roughness, B=Metallic
        vec3 mrSample = texture(material.metalRoughMap, TexCoord).rgb;
        roughnessValue = mrSample.g;
        metallicValue  = mrSample.b;

        // optionally, if AO is also in .r, you can use that 
        // if you don't have a separate AO map:
        // float aoFromMR = mrSample.r;
        // ...
    }
    else
    {
        // if separate
        if (material.hasMetallicMap)
        {
            // e.g. .r channel for metallic
            metallicValue = texture(material.metallicMap, TexCoord).r;
        }
        if (material.hasRoughnessMap)
        {
            roughnessValue = texture(material.roughnessMap, TexCoord).r;
        }
    }

    // 4) AO
    float aoValue = material.AO;
    if (material.hasAOMap)
    {
        aoValue = texture(material.AOMap, TexCoord).r;
    }

    // 5) Emissive
    vec3 emissiveColor = vec3(0.0);
    if (material.hasEmissiveMap)
    {
        emissiveColor = texture(material.emissiveMap, TexCoord).rgb;
        // if your emissive is sRGB or not depends on your pipeline
    }

    // 6) PBR Lighting
    vec3 V = normalize(viewPos - FragPos);

    // base reflectivity
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoColor, metallicValue);

    // accumulate
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < numPointLights; i++)
    {
        vec3 L = normalize(pointLights[i].position - FragPos);
        vec3 H = normalize(V + L);

        float distance    = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = pointLights[i].diffuse * attenuation;

        float NDF = DistributionGGX(N, H, roughnessValue);
        float G   = GeometrySmith(N, V, L, roughnessValue);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallicValue);

        float NdotL      = max(dot(N, L), 0.0);
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
        vec3 numerator    = NDF * G * F;
        vec3 specular     = numerator / denominator;

        Lo += (kD * albedoColor / PI + specular) * radiance * NdotL;
    }

    // minimal ambient
    vec3 ambient = 0.03 * albedoColor * aoValue;

    // final
    vec3 color = ambient + Lo + emissiveColor;
    // tone map
    vec3 R = reflect(-V, N);
    vec3 envColor = texture(environmentMap, R).rgb;

// combine it with your existing color
// you might scale it by a factor or a Fresnel, etc.
    color += envColor * 0.3; // or some factor

// tone map & gamma
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}

// --------------------------------------------------
// Helpers ...
// --------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2= NdotH*NdotH;

    float denom = (NdotH2*(a2-1.0)+1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness+1.0);
    float k = (r*r)/8.0;
    float num   = NdotV;
    float denom = NdotV*(1.0 - k) + k;
    return num/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1  = GeometrySchlickGGX(NdotV, roughness);
    float ggx2  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0)*pow(1.0 - cosTheta, 5.0);
}
