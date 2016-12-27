#include "console.h"

#define _SHADE_EPSILON .3  // Stop rendering text at this alpha

Console::Console(void) {
};


Console::Console(Font* regular_font, Font* strong_emphasis_font,
		 int line_spacing, int line_duration,
		 int fade_duration, int console_height,
		 int margin_left, int margin_bottom,
		 int buffer_length,
		 int screen_width, int screen_height,
		 bool from_right) {
	mainFont = regular_font;
	titleFont = strong_emphasis_font;
	lineSpacingInit = line_spacing;
	lineSpacing = lineSpacingInit;
	lineDuration = line_duration;
	fadeDuration = fade_duration;
	heightInit = console_height;
	height = heightInit;
	marginLeftInit = margin_left;
	marginLeft = marginLeftInit;
	marginBottomInit = margin_bottom;
	marginBottom = marginBottomInit;
	bufferLength = buffer_length;
	screenWidthInit = screen_width;
	screenHeightInit = screen_height;
	screenWidth = screenWidthInit;
	screenHeight = screenHeightInit;
	fromRight = from_right;
	textScale = 1.;
	isEnabled = 1;
	commandInput = "";
};


Console::~Console(void) { };


void Console::draw(GLuint shader_program, int current_frame,
		   int window_width, int window_height, bool input_mode) {
	if (!isEnabled) { return; };
	int _sz = textBuffer.size();
	int _linesAvailable = (height - marginBottom) / lineSpacing;
	if (bufferLength != CONSOLE_NONE) {
		_linesAvailable = (bufferLength < _linesAvailable) ?
			bufferLength : _linesAvailable;
	}		
	int _clippedLines = textBuffer.size() - _linesAvailable;
	int _collapseLines = (_linesAvailable > _sz) ?
		_linesAvailable - _sz : 0;
	int linesToRemove = (_clippedLines > 0) ? _clippedLines : 0;
	int i;
	float _textScale = textScale / FONT_UPSAMPLE;
	int _marginLeft = (fromRight) ? screenWidth - marginLeft : marginLeft;
	for (i = 0; i < _sz; i++) {
		GLfloat _offset = window_height - height +
			marginBottom +
			(i + _collapseLines) * lineSpacing;
		int _frameDelta = current_frame -
			lineCreationFrames[_sz - (i + 1)];
		if (_frameDelta > lineDuration) {
			++linesToRemove;
		} else if (i < _linesAvailable) {
			if (_frameDelta >
			    (lineDuration - fadeDuration)) {
				GLfloat _shade = _SHADE_EPSILON +
					(1 - _SHADE_EPSILON) *
					(1 - (_frameDelta -
					      (lineDuration - fadeDuration)) /
					 (float)fadeDuration);
				mainFont->RenderText(shader_program,
						     textBuffer[_sz -
								(i + 1)].c_str(),
						     _marginLeft,
						     _offset,
						     _textScale,
						     glm::vec4(_shade),
						     window_width, window_height);
			} else {
				mainFont->RenderText(shader_program,
						     textBuffer[_sz -
								(i + 1)].c_str(),
						     _marginLeft,
						     _offset,
						     _textScale,
						     glm::vec4(1.),
						     window_width,
						     window_height);
			}
		}
	}
	if ((commandInput != "") && input_mode) {
		GLfloat _offset = window_height - height + marginBottom +
			(_collapseLines - 1) * lineSpacing;
		mainFont->RenderText(shader_program,
				     commandInput.c_str(),
				     _marginLeft,
				     _offset,
				     _textScale,
				     glm::vec4(1.),
				     window_width,
				     window_height);
	}
	int _removeLCF = (linesToRemove > lineCreationFrames.size()) ?
		lineCreationFrames.size() : linesToRemove;
	for (int i = 0; i < _removeLCF; i++) 
		lineCreationFrames.erase(lineCreationFrames.begin());
	int _removeTB = (linesToRemove > textBuffer.size()) ?
		textBuffer.size() : linesToRemove;
	for (int i = 0; i < _removeTB; i++)
		textBuffer.erase(textBuffer.begin());
}


void Console::enable(void) {
	isEnabled = 1;
}


void Console::disable(void) {
	isEnabled = 0;
	for (int i = 0; i < textBuffer.size(); i++) 
		textBuffer.pop_back();
	for (int i = 0; i < lineCreationFrames.size(); i++)
		lineCreationFrames.pop_back();
}


bool Console::status(void) {
	return isEnabled;
}


void Console::write(std::string text, int creationFrame) {
	textBuffer.push_back(text);
	lineCreationFrames.push_back(creationFrame);
}


void Console::input(char c) {
	commandInput = commandInput + c;
}


void Console::resizeWindow(int screen_width, int screen_height) {
	float _sdh = 1 +
		(float)(screen_height - screenHeightInit) / screenHeightInit;
	float _sdw = 1 +
		(float)(screen_width - screenWidthInit) / screenWidthInit;
	textScale = _sdh;
	lineSpacing = _sdh * lineSpacingInit;
	height = _sdh * heightInit;
	marginBottom = _sdh * marginBottomInit;
	marginLeft = textScale * marginLeftInit;
	screenWidth = screen_width;
	screenHeight = screen_height;
}
