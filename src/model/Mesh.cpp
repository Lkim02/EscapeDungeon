
#include "Mesh.h"
#include "utils.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures):
			vertices(vertices), indices(indices), textures(textures) {
	setup();
}

Mesh::~Mesh() {

}

void Mesh::draw(Shader* shader) {
	/* Set Texture Units */
	uint32_t diffuse_num = 1; // at least 16, type can be smaller
	uint32_t specular_num = 1;
	uint32_t normal_num = 1;
	uint32_t height_num = 1;
	for (int i = 0; i < textures.size(); i++) {
		GL_CALL(glActiveTexture(GL_TEXTURE0 + i)); // activate texture unit i
		
		// set the sampler to the correct texture unit
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")	number = std::to_string(diffuse_num++);
		else if (name == "texture_specular") number = std::to_string(specular_num++);
		else if (name == "texture_normal") number = std::to_string(normal_num++);
		else if (name == "texture_height") number = std::to_string(height_num++);
		shader->setInt("material." + name + number, i);  // bind "material.texture_xxn" sampler (sample2D) to texture unit i

		GL_CALL(glBindTexture(GL_TEXTURE_2D, textures[i].id)); // bind to textures[i] to texture unit i
	}
	GL_CALL(glActiveTexture(GL_TEXTURE0)); // reset to default

	/* Draw Mesh */
	glBindVertexArray(_vao);
	GL_CALL(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
	glBindVertexArray(0);
}

void Mesh::setup() {
	/* Create VAO, VBO and EBO */
	GL_CALL(glGenVertexArrays(1, &_vao));
	GL_CALL(glGenBuffers(1, &_vbo));
	GL_CALL(glGenBuffers(1, &_ebo));

	/* Bind VAO */
	setVAO(_vao, {
		/*=== init vbo and ebo data ===*/
		// bind vbo and send data
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));
		// bind ebo and send data
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW));
		
		/*=== connect vbo with vao ===*/
		// vertex position
		GL_CALL(glEnableVertexAttribArray(0));
		GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
		// vertex normal
		GL_CALL(glEnableVertexAttribArray(1));
		GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));
		// vertex texture coordinate
		GL_CALL(glEnableVertexAttribArray(2));
		GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord)));
		// bone id
		GL_CALL(glEnableVertexAttribArray(3));
		GL_CALL(glVertexAttribIPointer(3, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, bone_ids)));
		// bone weight
		GL_CALL(glEnableVertexAttribArray(4));
		GL_CALL(glVertexAttribPointer(4, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights)));
	});
}

void Mesh::printVertices() {
	for (int i = 0; i < vertices.size(); i++) {
		std::cout << "Vertex[" << i << "]: " << std::endl;
		std::cout << "\tPosition: " << vertices[i].position.x << " " << vertices[i].position.y << " " << vertices[i].position.z << std::endl;
		//std::cout << "\tNormal: " << vertices[i].normal.x << " " << vertices[i].normal.y << " " << vertices[i].normal.z << std::endl;
		//std::cout << "\tTexcoord: " << vertices[i].texcoord.x << " " << vertices[i].texcoord.y << std::endl;
		std::cout << "\tBone Ids: ";
		for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
			std::cout << vertices[i].bone_ids[j] << " ";
		}
		std::cout << std::endl;
		std::cout << "\tWeights: ";
		for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
			std::cout << vertices[i].weights[j] << " ";
		}
		std::cout << std::endl;
	}
}