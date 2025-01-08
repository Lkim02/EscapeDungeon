#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include <string>
//#include <iostream>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

class Shader {
public:
	Shader(const char* vertex_path, const char* fragment_path);
	~Shader() = default;

	void begin(); // begin to use shader
	void end();   // end to use shader

	void setFloat(const std::string& name, float value) const;
	void setVec3f(const std::string& name, float x, float y, float z) const;
	void setVec3f(const std::string& name, const float* value) const;
	void setInt(const std::string& name, int value) const;  // for texture
	void setMat3f(const std::string& name, const float* value) const;
	void setMat4f(const std::string& name, const float* value) const;

private:
	GLuint _program = 0;

	void checkShaderErrors(GLuint target, int type); // type = 0: compile error, type = 1: link error

};

#endif // !SHADER_H
