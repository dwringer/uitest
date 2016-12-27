#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


TextureLoader::TextureLoader(void) {
}

TextureLoader::~TextureLoader(void) {
}

GLuint TextureLoader::LoadTexture(const char* filename) {
	GLuint textureID;
	int w, h, numComponents;
	unsigned char* image = stbi_load(filename, &w, &h, &numComponents,
					 STBI_rgb_alpha);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);
	return textureID;
}
