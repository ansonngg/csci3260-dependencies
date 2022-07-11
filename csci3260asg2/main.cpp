/*
Student Information
Student ID: 1155110182
Student Name: NG Ka Wai
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

Shader shader;
Shader shadowShader;

// Camera
glm::vec3 eye(0.0f, 2.0f, 7.7f);
glm::vec3 center(0.0f, -1.0f, 0.0f);
float yaw = glm::radians(90.0f);
float pitch = glm::radians(21.3f);
float zoomSpeed = 0.1f;
// Dolphin
glm::vec3 dolphinPos(0.0f, -1.0f, 0.0f);
float dolphinAngle = glm::radians(90.0f);
float dolphinSpeed = 2.0f;
float dolphinRotate = 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct KeyboardController {
	int theme_dolphin = 0;
	int theme_sea = 0;
	float intensity = 1.0f;
	bool UP = false;
	bool DOWN = false;
	bool LEFT = false;
	bool RIGHT = false;
};

KeyboardController keyboardCtl;

struct MouseController {
	bool LEFT_BUTTON = false;
	double oldx = 0.0, oldy = 0.0;
	float sensitivity = 0.01f;
};

MouseController mouseCtl;

// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

Model dolphinObj = loadOBJ("resources/dolphin/dolphin.obj");
Texture dolphinTexture[2];
Model seaObj = loadOBJ("resources/sea/sea.obj");
Texture seaTexture[2];
Model flashlightObj = loadOBJ("resources/flashlight/flashlight.obj");
Texture flashlightTexture;
Model sphereObj = loadOBJ("resources/sphere/3d-model.obj");
Model moonObj = loadOBJ("resources/moon/Moon.obj");
Texture moonTexture;
GLuint vao[5];
GLuint vbo[5];
GLuint ebo[5];
GLuint shadowFrameBuffer = 0;
GLuint depthTexture;

void sendDataToOpenGL()
{
	//TODO
	//Load objects and bind to VAO and VBO
	//Load textures
	glGenVertexArrays(5, vao);
	glGenBuffers(5, vbo);
	glGenBuffers(5, ebo);

	// 0 Dolphin
	dolphinTexture[0].setupTexture("resources/dolphin/dolphin_01.jpg");
	dolphinTexture[1].setupTexture("resources/dolphin/dolphin_02.jpg");
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, dolphinObj.vertices.size() * sizeof(Vertex), &dolphinObj.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dolphinObj.indices.size() * sizeof(unsigned int), &dolphinObj.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	// 1 Sea
	seaTexture[0].setupTexture("resources/sea/sea_01.jpg");
	seaTexture[1].setupTexture("resources/sea/sea_02.jpg");
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, seaObj.vertices.size() * sizeof(Vertex), &seaObj.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, seaObj.indices.size() * sizeof(unsigned int), &seaObj.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	// 2 Flashlight
	flashlightTexture.setupTexture("resources/flashlight/lighter-lo_Base_Color.PNG");
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, flashlightObj.vertices.size() * sizeof(Vertex), &flashlightObj.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, flashlightObj.indices.size() * sizeof(unsigned int), &flashlightObj.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	// 3 Sphere
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sphereObj.vertices.size() * sizeof(Vertex), &sphereObj.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereObj.indices.size() * sizeof(unsigned int), &sphereObj.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	// 4 Moon
	moonTexture.setupTexture("resources/moon/az97f-vlj9k.png");
	glBindVertexArray(vao[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, moonObj.vertices.size() * sizeof(Vertex), &moonObj.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, moonObj.indices.size() * sizeof(unsigned int), &moonObj.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	// Shadow mapping
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "FrameBuffer does not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	//TODO: set up the camera parameters	
	//TODO: set up the vertex shader and fragment shader
	shader.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
	shadowShader.setupShader("ShadowVertexShader.glsl", "ShadowFragmentShader.glsl");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void paintGL(void)  //always run
{
	/* Shadow Part */
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shadowShader.use();

	// Shadow mapping
	glm::vec3 lightInvDir = glm::vec3(-5.0f, 6.0f, -5.0f);
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-5, 5, -5, 10, 0, 20);
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix;
	shadowShader.setMat4("depthMVP", depthMVP);

	// 0 Dolphin
	glBindVertexArray(vao[0]);
	glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f) - dolphinAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), dolphinPos) * rotateMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
	shadowShader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, dolphinObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 1 Sea
	glBindVertexArray(vao[1]);
	modelMatrix = glm::mat4(1.0f);
	shadowShader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, seaObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 2 Flashlight
	glBindVertexArray(vao[2]);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	shadowShader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, flashlightObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 3 Sphere 1
	glBindVertexArray(vao[3]);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.26f, 2.8f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.0005f, 0.0005f, 0.0005f));
	shadowShader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 3 Sphere 2
	glBindVertexArray(vao[3]);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, -3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
	shadowShader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 3 Sphere 3
	glBindVertexArray(vao[3]);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
	shadowShader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 4 Moon
	glBindVertexArray(vao[4]);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 6.0f, -6.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	shadowShader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, moonObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* Usual Part */
	glClearColor(20.0f / 255.0f, 24.0f / 255.0f, 82.0f / 255.0f, 1.0f);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.use();

	// viewMatrix
	glm::mat4 viewMatrix = glm::lookAt(eye, center, glm::vec3(0.0f, 1.0f, 0.0f));
	shader.setMat4("viewMatrix", viewMatrix);
	// projectionMatrix
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 8.0f / 6.0f, 0.1f, 100.0f);
	shader.setMat4("projectionMatrix", projectionMatrix);
	// eyePosition
	glm::vec3 eyePosition(eye);
	shader.setVec3("eyePosition", eyePosition);

	// Directional light
	shader.setVec3("dirLight.direction", 5.0f, -6.0f, 5.0f);
	shader.setFloat("dirLight.intensity", keyboardCtl.intensity);
	shader.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
	shader.setVec3("dirLight.diffuse", 79.0f / 255.0f, 105.0f / 255.0f, 136.0f / 255.0f);
	shader.setVec3("dirLight.specular", 79.0f / 255.0f, 105.0f / 255.0f, 136.0f / 255.0f);
	// Point light 1
	shader.setVec3("pointLight[0].position", 0.0f, 0.26f, 2.8f);
	shader.setFloat("pointLight[0].constant", 1.0f);
	shader.setFloat("pointLight[0].linear", 0.09f);
	shader.setFloat("pointLight[0].quadratic", 0.032f);
	shader.setVec3("pointLight[0].ambient", 0.01f, 0.01f, 0.01f);
	shader.setVec3("pointLight[0].diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
	// Point light 2
	shader.setVec3("pointLight[1].position", -3.0f, -0.8f, -3.0f);
	shader.setFloat("pointLight[1].constant", 1.0f);
	shader.setFloat("pointLight[1].linear", 0.09f);
	shader.setFloat("pointLight[1].quadratic", 0.032f);
	shader.setVec3("pointLight[1].ambient", 0.5f, 0.05f, 0.05f);
	shader.setVec3("pointLight[1].diffuse", 1.0f, 0.1f, 0.1f);
	shader.setVec3("pointLight[1].specular", 1.0f, 0.1f, 0.1f);
	// Point light 3
	shader.setVec3("pointLight[2].position", 3.0f, -0.8f, -3.0f);
	shader.setFloat("pointLight[2].constant", 1.0f);
	shader.setFloat("pointLight[2].linear", 0.09f);
	shader.setFloat("pointLight[2].quadratic", 0.032f);
	shader.setVec3("pointLight[2].ambient", 0.05f, 0.05f, 0.5f);
	shader.setVec3("pointLight[2].diffuse", 0.1f, 0.1f, 1.0f);
	shader.setVec3("pointLight[2].specular", 0.1f, 0.1f, 1.0f);
	// Spot light
	shader.setVec3("spotLight.position", 0.0f, 0.26f, 2.8f);
	shader.setVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
	shader.setFloat("spotLight.cutOff", 0.9978f);
	shader.setFloat("spotLight.outerCutOff", 0.953f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.0f);
	shader.setFloat("spotLight.quadratic", 0.001f);
	shader.setVec3("spotLight.ambient", 0.01f, 0.01f, 0.01f);
	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

	// Dolphin movement
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if (keyboardCtl.UP) {
		dolphinPos.x += glm::cos(dolphinAngle) * dolphinSpeed * deltaTime;
		dolphinPos.z += glm::sin(dolphinAngle) * dolphinSpeed * deltaTime;
	}
	if (keyboardCtl.DOWN) {
		dolphinPos.x -= glm::cos(dolphinAngle) * dolphinSpeed * deltaTime;
		dolphinPos.z -= glm::sin(dolphinAngle) * dolphinSpeed * deltaTime;
	}
	if (keyboardCtl.LEFT)
		dolphinAngle -= dolphinRotate * deltaTime;
	if (keyboardCtl.RIGHT)
		dolphinAngle += dolphinRotate * deltaTime;

	// Shadow mapping
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
	shader.setMat4("depthBiasMVP", depthBiasMVP);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	shader.setInt("shadowMap", 1);
	shader.setVec3("lightInvDir", glm::normalize(lightInvDir));

	// 0 Dolphin
	glBindVertexArray(vao[0]);
	dolphinTexture[keyboardCtl.theme_dolphin].bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setFloat("material.shininess", 20);
	shader.setInt("material.isLight", 0);
	rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f) - dolphinAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::translate(glm::mat4(1.0f), dolphinPos) * rotateMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, dolphinObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 1 Sea
	glBindVertexArray(vao[1]);
	seaTexture[keyboardCtl.theme_sea].bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setFloat("material.shininess", 30);
	shader.setInt("material.isLight", 0);
	modelMatrix = glm::mat4(1.0f);
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, seaObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 2 Flashlight
	glBindVertexArray(vao[2]);
	flashlightTexture.bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setFloat("material.shininess", 50);
	shader.setInt("material.isLight", 0);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, flashlightObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 3 Sphere 1
	glBindVertexArray(vao[3]);
	shader.setFloat("material.shininess", 1);
	shader.setInt("material.isLight", 1);
	shader.setVec3("material.lightColor", 1.0f, 1.0f, 1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.26f, 2.8f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.0005f, 0.0005f, 0.0005f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 3 Sphere 2
	glBindVertexArray(vao[3]);
	shader.setFloat("material.shininess", 1);
	shader.setInt("material.isLight", 1);
	shader.setVec3("material.lightColor", 1.0f, 0.1f, 0.1f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, -3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 3 Sphere 3
	glBindVertexArray(vao[3]);
	shader.setFloat("material.shininess", 1);
	shader.setInt("material.isLight", 1);
	shader.setVec3("material.lightColor", 0.1f, 0.1f, 1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 4 Moon
	glBindVertexArray(vao[4]);
	moonTexture.bind(0);
	shader.setInt("material.myTexture", 0);
	shader.setFloat("material.shininess", 10);
	shader.setInt("material.isLight", 0);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 6.0f, -6.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("modelMatrix", modelMatrix);
	glDrawElements(GL_TRIANGLES, moonObj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Sets the mouse-button callback for the current window.
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouseCtl.LEFT_BUTTON = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		mouseCtl.LEFT_BUTTON = false;
}

bool firstMouse = true;

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	// Sets the cursor position callback for the current window
	if (firstMouse) {
		mouseCtl.oldx = x;
		mouseCtl.oldy = y;
		firstMouse = false;
	}
	if (mouseCtl.LEFT_BUTTON) {
		float radius = glm::distance(eye, center);
		float hspeed = mouseCtl.sensitivity * (x - mouseCtl.oldx);
		float vspeed = mouseCtl.sensitivity * (y - mouseCtl.oldy);
		yaw += hspeed;
		pitch += vspeed;
		if (pitch > glm::radians(89.0f)) {
			pitch = glm::radians(89.0f);
		}
		if (pitch < glm::radians(-89.0f)) {
			pitch = glm::radians(-89.0f);
		}
		glm::vec3 front;
		front.x = glm::cos(yaw) * glm::cos(pitch);
		front.y = glm::sin(pitch);
		front.z = glm::sin(yaw) * glm::cos(pitch);
		eye = center + front * radius;
	}
	mouseCtl.oldx = x;
	mouseCtl.oldy = y;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	eye += (float)yoffset * zoomSpeed * (center - eye);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Sets the Keyboard callback for the current window.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		keyboardCtl.theme_dolphin = 0;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		keyboardCtl.theme_dolphin = 1;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		keyboardCtl.theme_sea = 0;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		keyboardCtl.theme_sea = 1;
	if (key == GLFW_KEY_W && action == GLFW_PRESS && keyboardCtl.intensity < 1.0f)
		keyboardCtl.intensity += 0.1f;
	if (key == GLFW_KEY_S && action == GLFW_PRESS && keyboardCtl.intensity > 0.0f)
		keyboardCtl.intensity -= 0.1f;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		keyboardCtl.UP = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		keyboardCtl.DOWN = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		keyboardCtl.LEFT = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		keyboardCtl.RIGHT = true;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		keyboardCtl.UP = false;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		keyboardCtl.DOWN = false;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
		keyboardCtl.LEFT = false;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		keyboardCtl.RIGHT = false;
}


int main(int argc, char* argv[])
{
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
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	initializedGL();

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






