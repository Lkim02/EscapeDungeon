
#include "utils.h"

void check_error() {
	GLenum error_code = glGetError();
	if (error_code == GL_NO_ERROR) return;
	
	std::string error_str = "";
	switch (error_code) {
	case GL_INVALID_ENUM :	error_str = "GL_INVALID_ENUM";	break;
	case GL_INVALID_VALUE:	error_str = "GL_INVALID_VALUE"; break;
	case GL_INVALID_OPERATION: error_str = "GL_INVALID_OPERATION";  break;
	case GL_OUT_OF_MEMORY:	error_str = "GL_OUT_OF_MEMORY"; break;
	default: 
		error_str = "UNKNOWN ERROR";
		break;
	}
	std::cout << error_str << std::endl;
	assert(false);
}

glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& scale) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	return model; // model = translate (* rotate) * scale
}

//glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
//	glm::mat4 model = glm::mat4(1.0f);
//	// translate
//	model = glm::translate(model, position);
//	// rotate
//	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // around x axis
//	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // around y axis
//	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // around z axis
//	// scale
//	model = glm::scale(model, scale);
//	return model; // // model = translate * rotate * scale
//}

glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up, const glm::vec3& right, const glm::vec3& scale) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0] = glm::vec4(right, 0.0f); 
	rotation[1] = glm::vec4(up, 0.0f);
	rotation[2] = glm::vec4(front, 0.0f); // TODO: check, whether front or -front
	model = model * rotation;
	
	model = glm::scale(model, scale);
	return model;
}

glm::mat4 createModelMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = model * glm::mat4_cast(rotation);
	model = glm::scale(model, scale);
	return model; // // model = translate * rotate * scale
}

std::unordered_map<int8_t, glm::vec3> pace_vec = {
	{ GLFW_KEY_W, glm::vec3( 0.0f, 0.0f, -1.0f) },
	{ GLFW_KEY_S, glm::vec3( 0.0f, 0.0f,  1.0f) },
	{ GLFW_KEY_A, glm::vec3(-1.0f, 0.0f,  0.0f) },
	{ GLFW_KEY_D, glm::vec3( 1.0f, 0.0f,  0.0f) }
};

std::vector<glm::vec3> generateRandomPoints (
	int n,
	float x_min, float x_max,
	float z_min, float z_max,
	float min_distance
) {
	std::vector<glm::vec2> points;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> x_dist(x_min, x_max);
	std::uniform_real_distribution<float> z_dist(z_min, z_max);

	while (points.size() < n) {
		glm::vec2 p = { x_dist(gen), z_dist(gen) };
		bool valid = true;

		// Check
		for (const glm::vec2& existing : points) {
			if (glm::distance(p, existing) < min_distance) {
				valid = false;
				break;
			}
		}

		if (valid) {
			points.push_back(p);
		}
	}

	std::vector<glm::vec3> vec3_points;
	std::uniform_real_distribution<float> y_dist(0.5f, 2.0f); // do not involved caculating distance
	std::transform(points.begin(), points.end(), std::back_inserter(vec3_points),
		[&](const glm::vec2& p) {
			return glm::vec3(p.x, y_dist(gen), p.y);
		}
	);

	return vec3_points;
}

std::vector<glm::vec3> generateRandomPoints(
	int n,
	float r_min, float r_max,
	float min_distance
) {
	std::vector<glm::vec2> points;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> radius_dist(r_min, r_max);
	std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * 3.14159f);

	while (points.size() < n) {
		// Using Polar Coordinate
		float r = radius_dist(gen);
		float theta = angle_dist(gen);
		
		glm::vec2 p = { r * cos(theta), r * sin(theta) };

		// Check
		bool valid = true;
		for (const auto& existing : points) {
			if (glm::distance(p, existing) < min_distance) {
				valid = false;
				break;
			}
		}

		if (valid) {
			points.push_back(p);
		}
	}

	std::vector<glm::vec3> vec3_points;
	std::uniform_real_distribution<float> y_dist(0.5f, 2.0f); // do not involved caculating distance
	std::transform(points.begin(), points.end(), std::back_inserter(vec3_points),
		[&](const glm::vec2& p) {
			return glm::vec3(p.x, y_dist(gen), p.y);
		}
	);

	return vec3_points;
}