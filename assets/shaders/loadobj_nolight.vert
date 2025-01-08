#version 460 core

#define MAX_BONE_INFLUENCE 4
#define MAX_BONES 100

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aBoneIds; 
layout(location = 4) in vec4 aWeights;
//layout(location = 3) in int aBoneIds[MAX_BONE_INFLUENCE]; 
//layout(location = 4) in float aWeights[MAX_BONE_INFLUENCE];

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 finalBonesMatrices[MAX_BONES];

/* Debug */
out vec4 finalPosition;

void main() {
    /* Animate Update */
    bool hasAnimation = false;
    vec4 totalPosition = vec4(0.0f);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if(aBoneIds[i] == -1) continue;
        hasAnimation = true;
        if(aBoneIds[i] >= MAX_BONES) {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];
        vec3 localNormal = mat3(finalBonesMatrices[aBoneIds[i]]) * aNormal;
    }

    if (!hasAnimation) totalPosition = vec4(aPos, 1.0f);

    finalPosition = totalPosition;
    TexCoords = aTexCoords;
    //gl_Position = projection * view * model * totalPosition;
    gl_Position = projection * view * model * totalPosition;
}