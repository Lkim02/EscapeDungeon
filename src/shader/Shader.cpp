
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>

#include "utils.h"

/* Constructor & Destructor */
Shader::Shader(const char* vertex_path, const char* fragment_path) {
	std::string vertex_code, fragment_code;
	std::ifstream vs_file, fs_file;

	/* Read Shader File */
	vs_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fs_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vs_file.open(vertex_path);
		fs_file.open(fragment_path);

		/* Get Gontent */
		std::stringstream vs_stream, fs_stream;
		vs_stream << vs_file.rdbuf();
		fs_stream << fs_file.rdbuf();

		vs_file.close();
		fs_file.close();

		/* Into code string */
		vertex_code = vs_stream.str();
		fragment_code = fs_stream.str();
	} catch (std::ifstream::failure& e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << e.what() << std::endl;
	}

	/* Create Shader */
    GLuint vertex_shader, fragment_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertex_code_src = vertex_code.c_str();
	const char* fragment_code_src = fragment_code.c_str();
    GL_CALL(glShaderSource(vertex_shader, 1, &vertex_code_src, nullptr));
    GL_CALL(glShaderSource(fragment_shader, 1, &fragment_code_src, nullptr));

	/* Compile Shader */
    glCompileShader(vertex_shader);
	checkShaderErrors(vertex_shader, 0);
    glCompileShader(fragment_shader);
	checkShaderErrors(fragment_shader, 0);

    /* Create Shader Program */
    _program = glCreateProgram();
    glAttachShader(_program, vertex_shader);
    glAttachShader(_program, fragment_shader);

    glLinkProgram(_program); // Link shader
	checkShaderErrors(_program, 1);

    /* Clear Seperate Raw Shader */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

//Shader::~Shader() {
//
//}


/* Main Operation */
void Shader::begin() {
	GL_CALL(glUseProgram(_program));
}


void Shader::end() {
	GL_CALL(glUseProgram(0));
}


void Shader::checkShaderErrors(GLuint target, int type) {
	int success = 0;
	char info_log[1024];
	if (type == 0) { // Check Compile Error
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(target, 1024, nullptr, info_log);
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
		}
	} else if (type == 1) { // Check Link Error
		glGetProgramiv(target, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(target, 1024, nullptr, info_log);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << info_log << std::endl;
		}
	}
}

void Shader::setFloat(const std::string& name, float value) const {
	GLint location = glGetUniformLocation(_program, name.c_str());
	GL_CALL(glUniform1f(location, value));
}

void Shader::setVec3f(const std::string& name, float x, float y, float z) const {
	GLint location = glGetUniformLocation(_program, name.c_str());
	GL_CALL(glUniform3f(location, x, y, z));
}

void Shader::setVec3f(const std::string& name, const float* values) const {
	GLint location = glGetUniformLocation(_program, name.c_str());
	GL_CALL(glUniform3fv(location, 1, values)); // values contain 1 vec3
}

void Shader::setInt(const std::string& name, int value) const {
	GLint location = glGetUniformLocation(_program, name.c_str());
	GL_CALL(glUniform1i(location, value));
}

void Shader::setMat3f(const std::string& name, const float* value) const {
	GLint location = glGetUniformLocation(_program, name.c_str());
	GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, value));
}

void Shader::setMat4f(const std::string& name, const float* value) const {
	GLint location = glGetUniformLocation(_program, name.c_str());
	GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, value));
}