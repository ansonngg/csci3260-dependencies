#pragma once
#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include "Model.h"

void Draw(GLuint** buffers, int index, Model model);

void CreateRockModel(glm::mat4* modelMatrices, GLuint amount);