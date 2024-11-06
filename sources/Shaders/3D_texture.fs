#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse;        // Diffuse texture (map_Kd)
    sampler2D specular;       // Specular texture (map_Ks)
    sampler2D ambient;        // Ambient texture (map_Ka)
    sampler2D shininessMap;   // Shininess texture (map_Ns)
    sampler2D transparencyMap; // Transparency texture (map_d)
    sampler2D bumpMap;        // Bump map (map_bump)
    vec3 ambientColor;        // Ambient color (Ka)
    vec3 diffuseColor;        // Diffuse color (Kd)
    vec3 specularColor;       // Specular color (Ks)
    float shininess;          // Shininess (Ns)
    float transparency;       // Transparency factor (d)
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

uniform Material material;
uniform int numDirLights;
uniform DirectionalLight dirLights[50];  // Maximum number of directional lights
uniform int numPointLights;
uniform PointLight pointLights[50];      // Maximum number of point lights
uniform vec3 viewPos;

void main()
{
    // Texture fetching
    vec3 color = texture(material.diffuse, TexCoord).rgb;
    vec3 ambient = texture(material.ambient, TexCoord).rgb; // Using ambient map or default color
    vec3 specular = texture(material.specular, TexCoord).rgb; // Specular map or default color
    float shininess = material.shininess;
    
    if (texture(material.shininessMap, TexCoord).r > 0.0) {
        shininess = texture(material.shininessMap, TexCoord).r * 128.0; // If shininess map is available, use it
    }
    
    // Bump mapping: Modify normal if bump map is available
    vec3 norm = normalize(Normal);
    if (texture(material.bumpMap, TexCoord).r > 0.0) {
        // Apply bump map if present (basic normal perturbation)
        vec3 bump = texture(material.bumpMap, TexCoord).rgb;
        bump = bump * 2.0 - 1.0; // Convert from [0,1] to [-1,1]
        norm = normalize(norm + bump * 0.1); // Apply the bump effect
    }

    // View direction
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate result color from lighting models
    vec3 result = vec3(0.0);

    // Directional Lights
    for (int i = 0; i < numDirLights; i++) {
        // Ambient lighting
        vec3 lightDir = normalize(-dirLights[i].direction);
        vec3 ambientLight = dirLights[i].ambient * ambient;

        // Diffuse lighting
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseLight = dirLights[i].diffuse * diff * color;

        // Specular lighting
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specularLight = dirLights[i].specular * spec * specular;

        // Accumulate results for this light
        result += ambientLight + diffuseLight + specularLight;
    }

    // Point Lights
    for (int i = 0; i < numPointLights; i++) {
        // Attenuation calculations
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));

        // Ambient lighting
        vec3 ambientLight = pointLights[i].ambient * ambient;

        // Diffuse lighting
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseLight = pointLights[i].diffuse * diff * color;

        // Specular lighting
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specularLight = pointLights[i].specular * spec * specular;

        // Apply attenuation
        ambientLight *= attenuation;
        diffuseLight *= attenuation;
        specularLight *= attenuation;

        // Accumulate results for this light
        result += ambientLight + diffuseLight + specularLight;
    }

    // Transparency: If a transparency map is provided, we use that; otherwise, use the `d` factor
    if (texture(material.transparencyMap, TexCoord).r < 1.0) {
        float transparency = texture(material.transparencyMap, TexCoord).r;
        FragColor = vec4(result, transparency);
    } else {
        FragColor = vec4(result, 1.0); // Full opacity
    }
}
