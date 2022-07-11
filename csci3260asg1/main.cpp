/*
Type your name and student ID here
	- Name: NG Ka Wai
	- Student ID: 1155110182
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"

#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>

GLint programID;
float x_eye = 0.0f;
float y_eye = 3.0f;
float z_eye = 5.0f;
float eye_hangle_delta = 0.01f;
float eye_vangle_delta = 0.01f;
float x_center = 0.0f;
float y_center = 3.0f;
float z_center = 0.0f;
float tree_scale = 1.0f;
float tree_x_delta = 0.0f;
float tree_z_delta = 0.0f;

void get_OpenGL_info() {
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID) {
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID) {
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) {
	std::ifstream meInput(fileName);
	if (!meInput.good()) {
		std::cout << "File failed to load ... " << fileName << std::endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders() {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	//adapter[0] = vertexShaderCode;
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	//adapter[0] = fragmentShaderCode;
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;
	
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

GLuint vao[6];
GLuint vbo[6];
GLuint vbo_idx[5];

void sendDataToOpenGL() {
	// TODO:
	// create 2D objects and 3D objects and/or lines (points) here and bind to VAOs & VBOs
	glGenVertexArrays(6, vao);
	glGenBuffers(6, vbo);
	glGenBuffers(5, vbo_idx);

	const GLfloat sun[] = {
		+21.0f, -1.0f, -18.0f, // position 0
		186.0f / 255.0f, 0.0f, 1.0f / 255.0f,

		+23.0f, -1.0f, -18.0f, // position 1
		249.0f / 255.0f, 215.0f / 255.0f, 28.0f / 255.0f,

		+23.0f, +1.0f, -18.0f, // position 2
		186.0f / 255.0f, 0.0f, 1.0f / 255.0f,

		+21.0f, +1.0f, -18.0f, // position 3
		249.0f / 255.0f, 215.0f / 255.0f, 28.0f / 255.0f,

		+21.0f, -1.0f, -20.0f, // position 4
		186.0f / 255.0f, 0.0f, 1.0f / 255.0f,

		+23.0f, -1.0f, -20.0f, // position 5
		249.0f / 255.0f, 215.0f / 255.0f, 28.0f / 255.0f,

		+23.0f, +1.0f, -20.0f, // position 6
		186.0f / 255.0f, 0.0f, 1.0f / 255.0f,

		+21.0f, +1.0f, -20.0f, // position 7
		249.0f / 255.0f, 215.0f / 255.0f, 28.0f / 255.0f
	};
	GLushort sun_idx[] = { 3, 1, 0, 3, 2, 1, 4, 5, 7, 5, 6, 7, 7, 3, 4, 3, 0, 4, 1, 2, 5, 2, 6, 5, 3, 7, 2, 7, 6, 2, 0, 1, 4, 1, 5, 4 };
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sun), sun, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_idx[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sun_idx), sun_idx, GL_STATIC_DRAW);
	glBindVertexArray(0);

	const GLfloat ground[] = {
		-20.0f, +0.0f, +20.0f, // position 0
		19.0f / 255.0f, 109.0f / 255.0f, 21.0f / 255.0f,

		-20.0f, +0.0f, -20.0f, // position 1
		19.0f / 255.0f, 109.0f / 255.0f, 21.0f / 255.0f,

		+20.0f, +0.0f, -20.0f, // position 2
		19.0f / 255.0f, 109.0f / 255.0f, 21.0f / 255.0f,

		+20.0f, +0.0f, +20.0f, // position 3
		19.0f / 255.0f, 109.0f / 255.0f, 21.0f / 255.0f
	};
	GLushort ground_idx[] = { 0, 1, 2, 2, 3, 0 };
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_idx[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ground_idx), ground_idx, GL_STATIC_DRAW);
	glBindVertexArray(0);

	const GLfloat moon[] = {
		-22.0f, -0.5f, -18.5f, // position 0
		201.0f / 255.0f, 201.0f / 255.0f, 201.0f / 255.0f,

		-21.0f, -0.5f, -18.5f, // position 1
		150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f,

		-21.0f, +0.5f, -18.5f, // position 2
		201.0f / 255.0f, 201.0f / 255.0f, 201.0f / 255.0f,

		-22.0f, +0.5f, -18.5f, // position 3
		150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f,

		-22.0f, -0.5f, -19.5f, // position 4
		201.0f / 255.0f, 201.0f / 255.0f, 201.0f / 255.0f,

		-21.0f, -0.5f, -19.5f, // position 5
		150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f,

		-21.0f, +0.5f, -19.5f, // position 6
		201.0f / 255.0f, 201.0f / 255.0f, 201.0f / 255.0f,

		-22.0f, +0.5f, -19.5f, // position 7
		150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f
	};
	GLushort moon_idx[] = { 3, 1, 0, 3, 2, 1, 4, 5, 7, 5, 6, 7, 7, 3, 4, 3, 0, 4, 1, 2, 5, 2, 6, 5, 3, 7, 2, 7, 6, 2, 0, 1, 4, 1, 5, 4 };
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(moon), moon, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_idx[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(moon_idx), moon_idx, GL_STATIC_DRAW);
	glBindVertexArray(0);

	const GLfloat sign[] = {
		-0.1f, +0.0f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f,

		-0.1f, +1.6f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f,

		+0.1f, +1.6f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f,

		+0.1f, +0.0f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f,

		-0.5f, +1.6f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f,

		-0.5f, +2.0f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f,

		+0.5f, +2.0f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f,

		+0.5f, +1.6f, +7.0f,
		139.0f / 255.0f, 90.0f / 255.0f, 43.0f / 255.0f
	};
	GLushort sign_idx[] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sign), sign, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_idx[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sign_idx), sign_idx, GL_STATIC_DRAW);
	glBindVertexArray(0);

	const GLfloat text[] = {
		+0.47f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.37f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.47f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.47f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.47f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.37f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.37f, +1.89f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.37f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.37f, +1.89f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.42f, +1.89f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.32f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.32f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.27f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.22f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.17f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.22f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.12f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.12f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.12f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.02f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.12f, +1.90f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.02f, +1.90f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.12f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.02f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.13f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.13f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.13f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.19f, +1.89f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.25f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.19f, +1.89f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.25f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.25f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.30f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.30f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.30f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.40f, +1.97f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.30f, +1.90f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.40f, +1.90f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.30f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.40f, +1.82f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.47f, +1.63f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.42f, +1.78f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.37f, +1.63f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.42f, +1.78f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.446f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.393f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.32f, +1.78f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.32f, +1.63f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.32f, +1.78f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.22f, +1.63f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.22f, +1.78f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.22f, +1.63f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.02f, +1.78f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.07f, +1.63f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.02f, +1.78f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.03f, +1.63f, +7.0f,
		255.0f, 255.0f, 255.0f,

		+0.046f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.007f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.19f, +1.73f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.19f, +1.72f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.19f, +1.70f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.18f, +1.67f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.21f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.22f, +1.68f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.23f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.22f, +1.68f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.23f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.24f, +1.68f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.25f, +1.71f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.24f, +1.68f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.27f, +1.73f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.27f, +1.72f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.27f, +1.70f, +7.0f,
		255.0f, 255.0f, 255.0f,

		-0.26f, +1.67f, +7.0f,
		255.0f, 255.0f, 255.0f,
	};
	glBindVertexArray(vao[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(text), text, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindVertexArray(0);

	const GLfloat tree[] = {
		-0.1f, +0.0f, +0.0f,
		139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f,

		-0.1f, +0.6f, +0.0f,
		139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f,

		+0.1f, +0.6f, +0.0f,
		139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f,

		+0.1f, +0.0f, +0.0f,
		139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f,

		-0.4f, +0.6f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		-0.2f, +0.9f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		+0.2f, +0.9f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		+0.4f, +0.6f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		-0.3f, +0.9f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		-0.1f, +1.2f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		+0.1f, +1.2f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		+0.3f, +0.9f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		-0.2f, +1.2f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		+0.0f, +1.5f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f,

		+0.2f, +1.2f, +0.0f,
		109.0f / 255.0f, 179.0f / 255.0f, 63.0f / 255.0f
	};
	GLushort tree_idx[] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14 };
	glBindVertexArray(vao[5]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tree), tree, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_idx[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tree_idx), tree_idx, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

glm::vec3 morning = glm::vec3(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f);
glm::vec3 night = glm::vec3(19.0f / 255.0f, 24.0f / 255.0f, 98.0f / 255.0f);
glm::vec3 current_sky = (morning + night) / 2.0f;  // Initialize to the time of sunrise
bool reverse = false;
float planets_angle = 0.0f;

void paintGL(void) {
	// always run
	// TODO:
	// render your objects and control the transformation here
	glm::vec3 change_of_sky = (morning - night) / 48300.0f;
	if (current_sky.x >= morning.x)
		reverse = true;
	else if (current_sky.x <= night.x)
		reverse = false;
	if (reverse)
		current_sky -= change_of_sky;
	else
		current_sky += change_of_sky;
	glClearColor(current_sky.x, current_sky.y, current_sky.z, 1.0f);  //specify the background color
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(x_eye, y_eye, z_eye), glm::vec3(x_center, y_center, z_center), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 8.0f / 6.0f, 0.1f, 100.0f);

	// Sun
	glBindVertexArray(vao[0]);
	planets_angle += glm::radians(360.0f / 96600.0f);
	if (planets_angle > glm::radians(360.0f))
		planets_angle -= glm::radians(360.0f);
	glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), planets_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	GLint modelMatrixUniformLocation = glGetUniformLocation(programID, "modelMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Ground
	glBindVertexArray(vao[1]);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	modelMatrixUniformLocation = glGetUniformLocation(programID, "modelMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Moon
	glBindVertexArray(vao[2]);
	modelMatrix = glm::rotate(glm::mat4(1.0f), planets_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	modelMatrixUniformLocation = glGetUniformLocation(programID, "modelMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sign
	glBindVertexArray(vao[3]);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	modelMatrixUniformLocation = glGetUniformLocation(programID, "modelMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Text
	glBindVertexArray(vao[4]);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	modelMatrixUniformLocation = glGetUniformLocation(programID, "modelMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 74);
	glBindVertexArray(0);

	// Tree
	glBindVertexArray(vao[5]);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tree_x_delta, 0.0f, tree_z_delta)) * glm::scale(glm::mat4(1.0f), glm::vec3(tree_scale));
	modelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	modelMatrixUniformLocation = glGetUniformLocation(programID, "modelMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// TODO:
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		tree_scale++;
	if (key == GLFW_KEY_E && action == GLFW_PRESS && tree_scale > 1.0f)
		tree_scale--;
	if (key == GLFW_KEY_W && action == GLFW_PRESS && tree_z_delta > -19.0f)  // To prevent the tree going outside the world
		tree_z_delta == 6.0f ? tree_z_delta -= 2 : tree_z_delta--;  // To prevent the tree staying the same x-axis as the eye
	if (key == GLFW_KEY_A && action == GLFW_PRESS && tree_x_delta > -19.0f)
		tree_x_delta--;
	if (key == GLFW_KEY_S && action == GLFW_PRESS && tree_z_delta < 19.0f)
		tree_z_delta == 4.0f ? tree_z_delta += 2 : tree_z_delta++;
	if (key == GLFW_KEY_D && action == GLFW_PRESS && tree_x_delta < 19.0f)
		tree_x_delta++;
}

double old_xpos, old_ypos;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (old_xpos && old_ypos) {
		glm::vec4 v = glm::vec4(glm::vec3(x_center, y_center, z_center), 1.0f);
		v = glm::translate(glm::mat4(1.0f), glm::vec3(-x_eye, -y_eye, -z_eye)) * v;  // Translate the eye to the origin
		float xspeed = eye_hangle_delta * (old_xpos - xpos);
		float yspeed = eye_vangle_delta * (old_ypos - ypos);
		v = glm::rotate(glm::mat4(1.0f), xspeed, glm::vec3(0.0f, 1.0f, 0.0f)) * v;  // To horizontally control the view

		// To vertically control the view
		float radius = glm::distance(glm::vec3(x_eye, y_eye, z_eye), glm::vec3(x_center, y_center, z_center));
		float eye_hangle = glm::asin(v.x / radius);
		eye_hangle = v.z > 0 ? glm::radians(180.0f) - eye_hangle : eye_hangle;
		v = glm::rotate(glm::mat4(1.0f), eye_hangle, glm::vec3(0.0f, 1.0f, 0.0f)) * v;  // Rotate the view towards z-negative
		v = glm::rotate(glm::mat4(1.0f), yspeed, glm::vec3(1.0f, 0.0f, 0.0f)) * v;
		float eye_vangle = glm::acos(glm::abs(v.y) / radius);
		// To prevent the view going over the head or the legs
		if (eye_vangle < 0.1f) {
			if (v.y < 0)
				v = glm::rotate(glm::mat4(1.0f), 0.1f - eye_vangle, glm::vec3(1.0f, 0.0f, 0.0f)) * v;
			else
				v = glm::rotate(glm::mat4(1.0f), eye_vangle - 0.1f, glm::vec3(1.0f, 0.0f, 0.0f)) * v;
		}

		// Go back to the original position
		v = glm::translate(glm::mat4(1.0f), glm::vec3(x_eye, y_eye, z_eye)) * glm::rotate(glm::mat4(1.0f), -eye_hangle, glm::vec3(0.0f, 1.0f, 0.0f)) * v;
		x_center = v.x;
		y_center = v.y;
		z_center = v.z;
	}
	old_xpos = xpos;
	old_ypos = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void initializedGL(void) {
	// run only once
	// TODO:
	sendDataToOpenGL();
	installShaders();
}

int main(int argc, char* argv[]) {
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 600, "Assignment 1", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Enable raw mouse motion
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	/* Initialize the glew */
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	get_OpenGL_info();
	initializedGL();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
