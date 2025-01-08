#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

/* Debug */
in vec4 finalPosition;

struct Material {
    // diffuse
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    // specular
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    // normal
    sampler2D texture_normal1;
    sampler2D texture_normal2;
    sampler2D texture_normal3;
}; 
uniform Material material;

void main() {    
    FragColor = mix(
        texture(material.texture_diffuse1, TexCoords),
        texture(material.texture_diffuse2, TexCoords),
        0.5
    );
    //FragColor = finalPosition;
}