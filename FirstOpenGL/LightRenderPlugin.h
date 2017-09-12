#ifndef LIGHTRENDERPLUGIN
#define LIGHTRENDERPLUGIN

#include "RenderPluginBase.h"
#include <memory>

enum OperationMode
{
	CAMERA = 0,
	EARTH,
	BOX
};

class Sphere;

class LightRenderPlugin : public RenderPluginBase
{
public:
	LightRenderPlugin();

	virtual void Render();
	virtual void PreRender();
	virtual void Keyboard(unsigned char key, int x, int y);
	virtual void MouseMotion(int x, int y);

protected:
	virtual void SetMatrix();

private:
	void InitializeLampMatrix();
	void InitializeEarthMatrix();

	void SetLampMatrix();
	void SetEarthMatrix();
	void SetFloorMatrix();//和基类使用相同的matrix

	//void Set2DTexture(GLuint* texture, const char* fileName);

	void SetUniformValue(GLuint program);
	void SetEarthUniformValue(GLuint program);

	void RenderObjects();

	void InitializeShadowResource();
	void RenderShadow();
	void SetShadowMatrix(const glm::mat4& modelMat);

	void RotateEarth(int x, int y);
	void RotateBox(int x, int y);

private:

	std::shared_ptr<Sphere> m_spSphere;

	GLuint containerVAO;
	GLuint lightVAO;
	GLuint floorVAO;
	GLuint earthVAO;

	GLuint floorTexture;
	GLuint earthTexture;

	glm::mat4 lampModelMat;
	glm::mat4 earthModelMat;

	glm::mat4 tempMat;
	glm::mat4 initialEarthMat;

	GLuint lampProgram;
	GLuint objProgram;
	GLuint floorProgram;

	//shadow relative
	glm::mat4 lightSpaceMat;
	GLuint shadowProgram;
	GLuint debugDepthProgram;
	GLuint shadowFBO;//into which shadow will be drew
	GLuint depthMap;

	OperationMode operationMode;
};

#endif // !LIGHTRENDERPLUGIN



