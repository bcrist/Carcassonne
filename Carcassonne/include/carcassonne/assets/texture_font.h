// Copyright (c) 2013 Dougrist Productions
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Benjamin Crist
// File: carcassonne/assets/texture_font.h

#ifndef CARCASSONNE_ASSETS_TEXTURE_FONT_H_
#define CARCASSONNE_ASSETS_TEXTURE_FONT_H_
#include "carcassonne/_carcassonne.h"

//#include "TextureLoader.h"

#define TEXTUREFONT_LIST_COUNT 256

namespace carcassonne {

namespace assets {

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

} // namespace assets

} // namespace carcassonne

#endif