#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <iostream>
#include <vector>

#include "textrender.h"

#define CONSOLE_NONE -1


class Console {
 public:
	Console(void);
	Console(Font*, Font*,
		int, int, int, int, int, int, int, int, int, bool);
	~Console(void);
	void draw(GLuint, int, int, int, bool);
	void write(std::string, int);
	void input(char);
	void resizeWindow(int, int);
	void enable(void);
	void disable(void);
	bool status(void);
 private:
	int height;
	int heightInit;
	int marginBottom;
	int marginLeft;
	int bufferLength;
	int lineSpacing;
	int lineDuration;
	int fadeDuration;
	int screenWidth;
	int screenHeight;
	int marginBottomInit;
	int marginLeftInit;
	int lineSpacingInit;
	int screenWidthInit;
	int screenHeightInit;
	bool fromRight;
	float textScale;
	std::vector< int > lineCreationFrames;
	std::vector< std::string > textBuffer;
	std::string commandInput;
	Font* mainFont;
	Font* titleFont;
	bool isEnabled;	
};

#endif
