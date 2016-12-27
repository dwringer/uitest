#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "vgl.h"
#include <iostream>
#include <cstring>

class TextureLoader {
 public:
	TextureLoader(void);
	~TextureLoader(void);
	GLuint LoadTexture(const char* filename);
};

#endif

