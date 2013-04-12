#include "TextureFont.h"

BMC_BEGIN

TextureFont::TextureFont(GLenum textureMode, GLuint texture, GLfloat *vertexColor, GLfloat baseline, int rows, int cols, TextureFontCharacterSpec *listsDefined, int numListsDefined)
{
	listBase = glGenLists(TEXTUREFONT_LIST_COUNT);

	initList = glGenLists(1);
	glNewList(initList, GL_COMPILE);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, textureMode);
		glBindTexture(GL_TEXTURE_2D, texture);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		if (vertexColor != NULL)
			glColor4fv(vertexColor);
		glListBase(listBase);
	glEndList();

	for (int i = 0; i < numListsDefined; i++)
	{
		TextureFontCharacterSpec &cs = listsDefined[i];

		this->baseline = baseline;
		charWidths[cs.code] = cs.width;
		GLfloat charS0 = cs.col / (GLfloat)cols;
		GLfloat charS1 = (cs.col + cs.width) / (GLfloat)cols;
		GLfloat charT0 = cs.row / (GLfloat)rows;
		GLfloat charT1 = (cs.row + 1) / (GLfloat)rows;
		GLfloat oneMinusBaseline = 1 - baseline;

		glNewList(listBase + cs.code, GL_COMPILE);
			if (cs.row >= 0)
			{
				glBegin(GL_QUADS);
					glTexCoord2f(charS0, charT0); glVertex3f(0, oneMinusBaseline, 0);
					glTexCoord2f(charS1, charT0); glVertex3f(cs.width, oneMinusBaseline, 0);
					glTexCoord2f(charS1, charT1); glVertex3f(cs.width, -baseline, 0);
					glTexCoord2f(charS0, charT1); glVertex3f(0, -baseline, 0);
				glEnd();
			}
			glTranslatef(cs.width, 0, 0);
		glEndList();
	}
}


TextureFont::~TextureFont()
{
	glDeleteLists(initList, 1);
	glDeleteLists(listBase, TEXTUREFONT_LIST_COUNT);
}

void TextureFont::metrics(const std::string &str, GLfloat scaleX, GLfloat scaleY, GLfloat &width, GLfloat &y0, GLfloat &y1) const
{
	GLfloat calcWidth = 0;
	GLfloat calcHeight = scaleY;

	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		unsigned char c = *it;
		if (c < TEXTUREFONT_LIST_COUNT)
			calcWidth += charWidths[c];
	}

	width = calcWidth * scaleX;
	y0 = scaleY * (1 - baseline);
	y1 = -scaleY * baseline;
}

void TextureFont::print(const std::string &str, GLfloat scaleX, GLfloat scaleY) const
{
	glCallList(initList);

	if (scaleX != 1 || scaleY != 1)
		glScalef(scaleX, scaleY, 1);

	glCallLists(str.length(), GL_BYTE, str.c_str());
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

BMC_END