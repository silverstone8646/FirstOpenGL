#ifndef IRENDERPLUGIN
#define IRENDERPLUGIN

#include "glew.h"

class IRenderPlugin
{
public:
	virtual void Render() = 0;
	virtual void PreRender() = 0;
	virtual void Keyboard(unsigned char key, int x, int y) = 0;
	virtual void MouseClick(int button, int state, int x, int y) = 0;
	virtual void MouseMotion(int x, int y) = 0;
	virtual void Reshape(GLsizei w, GLsizei h) = 0;
};

#endif // !IRENDERPLUGIN



