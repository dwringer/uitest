#ifndef __TEXTRENDER_H__
#define __TEXTRENDER_H__

#include <iostream>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "vgl.h"
#include "GL/wglew.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#define FONT_UPSAMPLE 4.

struct Character {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

class Font {
 public:
	Font(void);
	Font(std::string, float, FT_Library);
	~Font(void);
	std::map<GLchar, Character> Characters;
	void RenderText(GLuint, std::string, GLfloat, GLfloat, GLfloat,
			glm::vec4, int, int);
 private:
	GLuint VAO[1];
	GLuint VBO[1];
};

	


#endif
