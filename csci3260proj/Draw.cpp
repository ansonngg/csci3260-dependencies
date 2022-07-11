#include "Draw.h"

void Draw(GLuint** buffers, int index, Model model) {
	glBindVertexArray(buffers[0][index]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1][index]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), &model.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2][index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(unsigned int), &model.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void CreateRockModel(glm::mat4* modelMatrices, GLuint amount) {
	// Initialize random seed
	srand(glfwGetTime() * 1000.0f);
	GLfloat radius = 75.0f;
	GLfloat offset = 5.0f;
	GLfloat displacement;
	for (GLuint i = 0; i < amount; i++) {
		glm::mat4 model(1.0f);
		// 1. Translation: randomly displace along circle with radius 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		// x
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat x = glm::sin(angle) * radius + displacement;
		// y
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f + 1;
		// z
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat z = glm::cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));
		// 2. Scale: scale between 0.05 and 0.25f
		GLfloat scale = (rand() % 10) / 10.0f + 0.5;
		model = glm::scale(model, glm::vec3(scale));
		// 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
		GLfloat rotAngle = rand() % 360;
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		// 4. Now add to list of matrices
		modelMatrices[i] = model;
	}
}