#version 460 core
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

// Light
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

// Camera
uniform vec3 viewPos;

// Object
struct Material {
    vec3 ambient; 
    vec3 diffuse; 
    vec3 specular;
    float shininess;
}; 
uniform Material material;

void main() {
    vec3 norm = normalize(Normal);
    
    // Ambient
    vec3 ambient = material.ambient * light.ambient;

    // Diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;
    
    vec3 result = specular + diff + ambient;
    FragColor = vec4(result, 1.0);
}