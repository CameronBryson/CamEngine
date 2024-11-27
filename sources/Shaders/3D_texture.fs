#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// Material structure containing texture maps and material properties
struct Material {
    sampler2D diffuseMap;      // Diffuse texture (map_Kd)
    sampler2D specularMap;     // Specular texture (map_Ks)
    sampler2D ambientMap;      // Ambient texture (map_Ka)
    sampler2D normalMap;       // Normal map (map_bump)
    vec3 ambientColor;         // Ambient color (Ka)
    vec3 diffuseColor;         // Diffuse color (Kd)
    vec3 specularColor;        // Specular color (Ks)
    float shininess;           // Shininess (Ns)
    float transparency;        // Transparency factor (d)
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

vec3 applyBumpMapping(vec3 normal, vec2 texCoord) {
    // Sample the red channel of the normal map to check its availability
    if (texture(material.normalMap, texCoord).r > 0.0) {
        // Fetch the normal map value
        vec3 bump = texture(material.normalMap, texCoord).rgb;
        // Transform from [0,1] to [-1,1]
        bump = bump * 2.0 - 1.0;
        // Apply the bump effect with a scaling factor
        normal = normalize(normal + bump * 0.1);
    }
    return normal;
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float shininess) {
    // Calculate the direction vector of the light
    vec3 lightDir = normalize(-light.direction);
    
    // Ambient component
    vec3 ambient = light.ambient * texture(material.ambientMap, TexCoord).rgb;
    
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    
    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * specularColor;
    
    // Sum all components
    return ambient + diffuse + specular;
}


vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float shininess) {
    // Calculate the direction vector of the light
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Calculate the distance between the light and the fragment
    float distance = length(light.position - fragPos);
    
    // Calculate attenuation factor
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Ambient component
    vec3 ambient = light.ambient * texture(material.ambientMap, TexCoord).rgb;
    
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    
    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * specularColor;
    
    // Apply attenuation to all components
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    // Sum all components
    return ambient + diffuse + specular;
}

void main()
{
    // Fetch textures and material properties
    vec3 diffuseColor = texture(material.diffuseMap, TexCoord).rgb;
    vec3 ambientColor = texture(material.ambientMap, TexCoord).rgb;       // Ambient map or default color
    vec3 specularColor = texture(material.specularMap, TexCoord).rgb;     // Specular map or default color
    float shininess = material.shininess;

    // Apply bump mapping to modify the normal vector
    vec3 norm = normalize(Normal);
    norm = applyBumpMapping(norm, TexCoord);

    // Calculate the view direction vector
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Initialize the result color
    vec3 result = vec3(0.0);

    // Iterate through each directional light and accumulate lighting
    for (int i = 0; i < numDirLights; i++) {
        result += calculateDirectionalLight(dirLights[i], norm, viewDir, diffuseColor, specularColor, shininess);
    }

    // Iterate through each point light and accumulate lighting
    for (int i = 0; i < numPointLights; i++) {
        result += calculatePointLight(pointLights[i], norm, FragPos, viewDir, diffuseColor, specularColor, shininess);
    }

    // Apply transparency to the final color
    FragColor = vec4(result, material.transparency);
}
