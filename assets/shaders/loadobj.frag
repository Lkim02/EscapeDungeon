#version 460 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;

/*=== Light ===*/
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

/*=== Camera ===*/
uniform vec3 viewPos;

/*=== Object ===*/
struct Material {
    // vec3 ambient; 
    // vec3 diffuse; 
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    // vec3 specular;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    float shininess;
}; 
uniform Material material;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient
    vec3 ambient = texture(material.texture_diffuse1, TexCoords).rgb * light.ambient;

    // Diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = texture(material.texture_diffuse1, TexCoords).rgb * diff * light.diffuse;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = texture(material.texture_specular1, TexCoords).rgb * spec * light.specular;
    
    vec3 result = specular + diff + ambient;
    FragColor = vec4(result, 1.0);
}