#include "textrender.h"

Font::Font(void) { };

Font::Font(std::string filename, float font_size, FT_Library ft) {
	FT_Face face;
	if (FT_New_Face(ft, filename.c_str(), 0, &face)) exit(1);
	FT_Set_Pixel_Sizes(face, 0, font_size);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 129; c++) {
		if (c == 128) c = 186;
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) exit(1);
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D,
			     0,
			     GL_RED,
			     face->glyph->bitmap.width,
			     face->glyph->bitmap.rows,
			     0,
			     GL_RED,
			     GL_UNSIGNED_BYTE,
			     face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
				GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
				GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Character character = {texture,
				       glm::ivec2(face->glyph->bitmap.width,
						  face->glyph->bitmap.rows),
				       glm::ivec2(face->glyph->bitmap_left,
						  face->glyph->bitmap_top),
				       face->glyph->advance.x};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	};
	FT_Done_Face(face);

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL,
	 	     GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
};

Font::~Font(void) { };

void Font::RenderText(GLuint shader_program, std::string text,
		      GLfloat x, GLfloat y, GLfloat scale,
		      glm::vec4 color, int window_width, int window_height) {
	glUseProgram(shader_program);
	glUniform4f(glGetUniformLocation(shader_program, "textColor"),
		    color.x, color.y, color.z, color.w);
	glm::mat4 projection = glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"),
			   1, GL_FALSE, glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO[0]);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];
		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		GLfloat vertices[6][4] = {
			{xpos, ypos + h, 0., 0.},
			{xpos, ypos, 0., 1.},
			{xpos + w, ypos, 1., 1.},
			{xpos, ypos + h, 0., 0.},
			{xpos + w, ypos, 1., 1.},
			{xpos + w, ypos + h, 1., 0.}};
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

