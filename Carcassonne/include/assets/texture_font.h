#pragma once
#include "bmc.h"

#include "TextureLoader.h"

#define TEXTUREFONT_LIST_COUNT 256

BMC_BEGIN

struct TextureFontCharacterSpec
{
	GLubyte code;
	GLuint row;
	GLuint col;
	GLfloat width;
};

class TextureFont
{
public:
	TextureFont(GLenum textureMode, GLuint texture, GLfloat *vertexColor, GLfloat baseline, int rows, int cols, TextureFontCharacterSpec *listsDefined, int numListsDefined);
	~TextureFont();

	void metrics(const std::string &str, GLfloat scaleX, GLfloat scaleY, GLfloat &width, GLfloat &y0, GLfloat &y1) const;
	void print(const std::string &str, GLfloat scaleX, GLfloat scaleY) const;

private:
	GLuint listBase;	// first displaylist

	// displaylist for getting ready to print a string
	GLuint initList;

	// font info for calculating metrics
	GLfloat charWidths[TEXTUREFONT_LIST_COUNT];
	GLfloat baseline;
};

BMC_END