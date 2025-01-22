#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN; // Receive TBN matrix from vertex shader

// Material structure containing texture maps and material properties
struct Material {
    bool hasDiffuseMap;        // Flag to check if the diffuse map is available
    bool hasSpecularMap;       // Flag to check if the specular map is available
    bool hasAmbientMap;        // Flag to check if the ambient map is available
    bool hasNormalMap;         // Flag to check if the normal map is available
    bool hasRoughnessMap;      // Flag to check if the roughness map is available
    sampler2D diffuseMap;      // Diffuse texture (map_Kd)
    sampler2D specularMap;     // Specular texture (map_Ks)
    sampler2D ambientMap;      // Ambient texture (map_Ka)
    sampler2D normalMap;       // Normal map (map_bump)
    sampler2D roughnessMap;    // Roughness map
    vec3 ambientColor;         // Ambient color (Ka)
    vec3 diffuseColor;         // Diffuse color (Kd)
    vec3 specularColor;        // Specular color (Ks)
    vec3 emissiveColor;        // Emissive color (Ke)
    float shininess;           // Shininess (Ns)
    float opticalDensity;      // Optical density (Ni)
    float transparency;        // Transparency factor (d)
    int illum;                 // Illumination model (illum)
};

// Directional light structure
struct DirectionalLight {
    vec3 direction;   // Direction of the light
    vec3 ambient;     // Ambient color component
    vec3 diffuse;     // Diffuse color component
    vec3 specular;    // Specular color component
};

// Point light structure
struct PointLight {
    vec3 position;    // Position of the point light
    vec3 ambient;     // Ambient color component
    vec3 diffuse;     // Diffuse color component
    vec3 specular;    // Specular color component

    float constant;   // Attenuation constant
    float linear;     // Attenuation linear factor
    float quadratic;  // Attenuation quadratic factor
};

uniform Material material;
uniform int numDirLights;
uniform DirectionalLight dirLights[50];  // Maximum number of directional lights
uniform int numPointLights;
uniform PointLight pointLights[50];      // Maximum number of point lights
uniform vec3 viewPos;

// Function to apply normal mapping using the TBN matrix
vec3 applyNormalMapping(vec3 normal, vec2 texCoord) {
    if (material.hasNormalMap) {
        vec3 tangentNormal = texture(material.normalMap, TexCoord).rgb;
        tangentNormal = normalize(tangentNormal * 2.0 - 1.0); // Transform from [0,1] to [-1,1]

        // Transform tangent space normal to world space
        normal = normalize(TBN * tangentNormal);
    }
    return normal;
}

// Function to calculate directional light contribution using Blinn-Phong
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    
    // Ambient
    vec3 ambient = light.ambient * (material.hasAmbientMap ? texture(material.ambientMap, TexCoord).rgb : material.ambientColor);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * (material.hasDiffuseMap ? texture(material.diffuseMap, TexCoord).rgb : material.diffuseColor);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (material.hasSpecularMap ? texture(material.specularMap, TexCoord).rgb : material.specularColor);
    
    return ambient + diffuse + specular;
}

// Function to calculate point light contribution using Blinn-Phong
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Ambient
    vec3 ambient = light.ambient * (material.hasAmbientMap ? texture(material.ambientMap, TexCoord).rgb : material.ambientColor);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * (material.hasDiffuseMap ? texture(material.diffuseMap, TexCoord).rgb : material.diffuseColor);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (material.hasSpecularMap ? texture(material.specularMap, TexCoord).rgb : material.specularColor);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

void main()
{
    // Initialize normal from the TBN matrix's Normal component
    vec3 norm = normalize(TBN[2]);

    // Apply normal mapping to modify the normal vector
    norm = applyNormalMapping(norm, TexCoord);

    // Calculate the view direction vector
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Initialize the result color
    vec3 result = vec3(0.0);

    // Accumulate lighting from directional lights
    for (int i = 0; i < numDirLights; i++) {
        result += calculateDirectionalLight(dirLights[i], norm, viewDir);
    }

    // Accumulate lighting from point lights
    for (int i = 0; i < numPointLights; i++) {
        result += calculatePointLight(pointLights[i], norm, FragPos, viewDir);
    }

    // Gamma Correction
    float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    // Apply transparency to the final color
    FragColor = vec4(result, material.transparency);
}