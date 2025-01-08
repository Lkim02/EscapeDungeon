#ifndef UTILS_H
#define UTILS_H


#include <assert.h>
#include <unordered_map>
#include <cstdio>

#include "common.h"

/* Error Check */
#ifdef DEBUG

#define GL_CALL(glfunc) do { \
	glfunc; \
	check_error(); } while (false)

#else
	
#define GL_CALL(glfunc) glfunc;

#endif

void check_error();


/* Define */
#define setVAO(vao, operation)  do { \
		GL_CALL(glBindVertexArray(vao)); \
			operation  \
		GL_CALL(glBindVertexArray(0));   \
	} while (false)



glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3 & scale = glm::vec3(1.0f));
//inline glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale = glm::vec3(1.0f));
glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::vec3& front, const glm::vec3& up, const glm::vec3& right, const glm::vec3 & scale);
glm::mat4 createModelMatrix(const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale = glm::vec3(1.0f));

inline void printPxTransform(const physx::PxTransform& transform) {
    printf("Position: x = %f, y = %f, z = %f\n",
        transform.p.x, transform.p.y, transform.p.z);

    printf("Rotation (quaternion): x = %f, y = %f, z = %f, w = %f\n",
        transform.q.x, transform.q.y, transform.q.z, transform.q.w);
}

extern std::unordered_map<int8_t, glm::vec3> pace_vec;

std::vector<glm::vec3> generateRandomPoints(int n, float x_min, float x_max, float z_min, float z_max, float min_distance);
std::vector<glm::vec3> generateRandomPoints(int n, float r_min, float r_max, float min_distance);


#endif // !UTILS_H