#version 330 core

// ------------------------------
// Output Fragment Color
// ------------------------------
out vec4 FragColor;

// ------------------------------
// Input Variables from Vertex Shader
// ------------------------------
in vec2 TexCoord;     // Texture coordinates
in vec3 FragPos;      // Fragment position in world space
in mat3 TBN;          // Tangent-Bitangent-Normal matrix for normal mapping

// ------------------------------
// Material Structure
// ------------------------------
struct Material
{
    bool hasAlbedoMap;
    bool hasNormalMap;
    bool hasMetallicMap;
    bool hasRoughnessMap;
    bool hasAOMap;
    bool hasEmissiveMap;
    
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D AOMap;
    sampler2D emissiveMap;
    
    vec4 albedo;
    float metallic;
    float roughness;
    float AO;
};

// ------------------------------
// Directional Light Structure
// ------------------------------
struct DirectionalLight {
    vec3 direction;   // Direction of the light
    vec3 ambient;     // Ambient color component
    vec3 diffuse;     // Diffuse color component
    vec3 specular;    // Specular color component
};

// ------------------------------
// Point Light Structure
// ------------------------------
struct PointLight {
    vec3 position;    // Position of the point light
    vec3 ambient;     // Ambient color component
    vec3 diffuse;     // Diffuse color component
    vec3 specular;    // Specular color component
    
    float constant;   // Attenuation constant
    float linear;     // Attenuation linear factor
    float quadratic;  // Attenuation quadratic factor
};

// ------------------------------
// Uniform Variables
// ------------------------------
uniform Material material;               // Material properties
uniform int numDirLights;                // Number of directional lights
uniform DirectionalLight dirLights[50];  // Array of directional lights
uniform int numPointLights;              // Number of point lights
uniform PointLight pointLights[50];      // Array of point lights
uniform vec3 viewPos;                     // Camera position in world space

// ------------------------------
// Function Declarations
// ------------------------------
vec3 getNormal();
vec3 getAlbedo();
float getMetallic();
float getRoughness();
float getAO();
vec3 getEmissive();

// The following functions are retained but not utilized in this simplified shader
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

// Constants
const float pi = 3.14159265359;

// ------------------------------
// Main Function
// ------------------------------
void main()
{
    // Retrieve material properties
    vec3 albedoColor = getAlbedo();
    float ambientFactor = getAO();

    // Define a hardcoded ambient light color (e.g., soft white)
    vec3 ambientLight = vec3(0.4, 0.4, 0.4);

    // Calculate ambient lighting with Ambient Occlusion applied
    vec3 ambient = ambientLight * albedoColor * ambientFactor;

    // Initialize diffuse and specular components
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Check if there is at least one directional light
    if(numDirLights > 0)
    {
        // Use the first directional light for diffuse and specular lighting
        DirectionalLight light = dirLights[0];
        
        // Compute the light direction
        vec3 lightDir = normalize(-light.direction);
        
        // Calculate the normal vector
        vec3 normal = getNormal();
        
        // Calculate the view direction
        vec3 viewDir = normalize(viewPos - FragPos);
        
        // Compute the diffuse intensity using Lambertian reflectance
        float diff = max(dot(normal, lightDir), 0.0);
        
        // Calculate the diffuse color
        diffuse += light.diffuse * diff * albedoColor;
        
        // Compute the halfway vector for Blinn-Phong
        vec3 halfwayDir = normalize(lightDir + viewDir);
        
        // Calculate the specular intensity using Blinn-Phong
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0); // Shininess factor set to 32
        
        // Calculate the specular color
        specular += light.specular * spec;
    }
    // Combine ambient, diffuse, and specular components
    vec3 color = ambient + diffuse*material.albedo.a + specular;

    // Set the final fragment color
    FragColor = vec4(color, 1.0);
}

// ------------------------------
// Function Definitions
// ------------------------------

// Retrieves the normal vector, applying normal mapping if available
vec3 getNormal()
{
    vec3 normal = vec3(0.0, 0.0, 1.0); // Default normal

    if(material.hasNormalMap)
    {
        // Sample the normal map texture
        vec3 tangentNormal = texture(material.normalMap, TexCoord).rgb;
        // Transform the normal from [0,1] to [-1,1]
        tangentNormal = tangentNormal * 2.0 - 1.0;
        // Transform the normal vector to world space using the TBN matrix
        normal = normalize(TBN * tangentNormal);
    }
    else
    {
        // If no normal map, use the default normal
        normal = normalize(normal);
    }

    return normal;
}

// Retrieves the albedo color, using the albedo map if available
vec3 getAlbedo()
{
    if(material.hasAlbedoMap)
    {
        return pow(texture(material.albedoMap, TexCoord).rgb, vec3(2.2)); // Convert from sRGB to linear space
    }
    else
    {
        return material.albedo.rgb;
    }
}

// Retrieves the metallic factor, using the metallic map if available
float getMetallic()
{
    if(material.hasMetallicMap)
    {
        return texture(material.metallicMap, TexCoord).r;
    }
    else
    {
        return material.metallic;
    }
}

// Retrieves the roughness factor, using the roughness map if available
float getRoughness()
{
    if(material.hasRoughnessMap)
    {
        return texture(material.roughnessMap, TexCoord).r;
    }
    else
    {
        return material.roughness;
    }
}

// Retrieves the Ambient Occlusion factor, using the AO map if available
float getAO()
{
    if(material.hasAOMap)
    {
        return texture(material.AOMap, TexCoord).r;
    }
    else
    {
        return material.AO;
    }
}

// Retrieves the emissive color, using the emissive map if available
vec3 getEmissive()
{
    if(material.hasEmissiveMap)
    {
        return texture(material.emissiveMap, TexCoord).rgb;
    }
    else
    {
        return vec3(0.0);
    }
}

// The following functions are retained but not utilized in this simplified shader

// Calculates lighting contribution from a directional light
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
    // Function body remains unchanged
    return vec3(0.0);
}

// Calculates lighting contribution from a point light
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
    // Function body remains unchanged
    return vec3(0.0);
}

// Schlick's approximation for Fresnel factor
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX Normal Distribution Function
float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float numerator   = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
    denominator = pi * denominator * denominator;

    return numerator / denominator;
}

// Schlick-GGX Geometry Function for a single direction
float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float numerator   = NdotV;
    float denominator = NdotV * (1.0 - k) + k;

    return numerator / denominator;
}

// Smith's Geometry Function
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// ------------------------------
// End of Shader
// ------------------------------