#ifndef CONTAINERRENDERPLUGIN
#define CONTAINERRENDERPLUGIN


#include "RenderPluginBase.h"

class ContainerRenderPlugin : public RenderPluginBase
{
public:
	ContainerRenderPlugin();

	virtual void Render();
	virtual void PreRender();
	virtual void Keyboard(unsigned char key, int x, int y);

private:
	//void Set2DTexture(GLuint* texture, const char* fileName);

private:
	GLuint VAO;
	GLuint wallTexture;
	GLuint faceTexture;
};

#endif // !CONTAINERRENDERPLUGIN

