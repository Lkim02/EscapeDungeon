#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "shader/Shader.h"

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	int bone_ids[MAX_BONE_INFLUENCE];
	float weights[MAX_BONE_INFLUENCE];

	Vertex() {
		memset(bone_ids, -1, sizeof(bone_ids));
		memset(weights, 0.0f, sizeof(weights));
	}
};

struct Texture {
	GLuint id;
	std::string type;
	std::string path;
};


class Mesh {
public:
	/* Mesh Data */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	~Mesh();

	void draw(Shader* shader);

	/* Debug */
	void printVertices();

private:
	/* Render Data */
	GLuint _vao, _vbo, _ebo;
	void setup();

};


#endif // !MESH_H
