#include "Texture.h"

#include "Dependencies/glew/glew.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Dependencies/stb_image/stb_image.h"

#include <iostream>

void Texture::setupTexture(const char* texturePath)
{
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);
	// load the texture data into "data"
	unsigned char* data = stbi_load(texturePath, &Width, &Height, &BPP, 0);
	GLenum format = 3;
	switch (BPP) {
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture: " << texturePath << std::endl;
		exit(1);
	}

	std::cout << "Load " << texturePath << " successfully!" << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setupCubemap(const char** texturesFaces)
{
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	// load the texture data into "data"
	for (int i = 0; i < 6; i++) {
		unsigned char* data = stbi_load(texturesFaces[i], &Width, &Height, &BPP, 0);
		GLenum format = 3;
		switch (BPP) {
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		}
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Failed to load texture: " << texturesFaces[i] << std::endl;
			exit(1);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	std::cout << "Load cubemap successfully!" << std::endl;
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Texture::bind(unsigned int slot = 0) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::bindCubemap(unsigned int slot = 0) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
