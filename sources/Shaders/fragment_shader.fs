#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    vec3 ambient;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform int numDirLights;
uniform DirectionalLight dirLights[10]; // Maximum number of directional lights
uniform vec3 viewPos;

void main()
{
    vec3 color = texture(material.diffuse, TexCoord).rgb;
    vec3 result = vec3(0.0);

    for (int i = 0; i < numDirLights; i++) {
        // Ambient
        vec3 ambient = dirLights[i].ambient * color;

        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(-dirLights[i].direction);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = dirLights[i].diffuse * diff * color;

        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = dirLights[i].specular * spec * texture(material.specular, TexCoord).rgb;

        result += ambient + diffuse + specular;
    }

    FragColor = vec4(result, 1.0);
}