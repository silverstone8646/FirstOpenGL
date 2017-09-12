#ifndef RENDERPLUGINBASE
#define RENDERPLUGINBASE

#include "IRenderPlugin.h"
#include "defines.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "Camera.h"

class RenderPluginBase : public IRenderPlugin
{
public:
	RenderPluginBase();
	virtual void Render() = 0;
	virtual void PreRender() = 0;
	virtual void Keyboard(unsigned char key, int x, int y) = 0;
	virtual void MouseClick(int button, int state, int x, int y);
	virtual void MouseMotion(int x, int y);
	void Reshape(GLsizei w, GLsizei h);

protected:
	virtual void InitializeMatrix();
	virtual void SetMatrix();
	void Set2DTexture(GLuint* texture, const char* fileName);
	void SetDrawFlag();

protected:
	int windowWidth;
	int windowHeight;
	bool drawFlag;

	GLuint currentProgram;

	//mouse
	int mouseState;
	int mouse;
	int lastX;
	int lastY;

	//position
	float yaw; //航偏角
	float pitch; //俯仰角
	//float worldCenterToCamera;//世界坐标中心到相机的距离

							  //matrix
	glm::mat4 modelMat;
	glm::mat4 viewMat;
	glm::mat4 projectMat;

	Camera* camera;
};

#endif // !RENDERPLUGINBASE


