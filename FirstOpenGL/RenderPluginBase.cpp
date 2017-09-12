#include "RenderPluginBase.h"

#include "SOIL.h"

RenderPluginBase::RenderPluginBase() :
	windowWidth(800), windowHeight(600), drawFlag(true),
	mouseState(MouseState::Up), mouse(Mouse::Left),
	yaw(90.0f), pitch(0.0f)
{
	camera = new Camera();
}

//left mouse: 0, middle mouse: 1, right mouse: 2
//mousedown: 0, mouse up: 1
void RenderPluginBase::MouseClick(int button, int state, int x, int y)
{
	mouse = button;
	mouseState = state;
	if (mouseState == MouseState::Down)
	{
		lastX = x;
		lastY = y;
	}
}

void RenderPluginBase::MouseMotion(int x, int y)
{
	float sensitivity = 0.05f;

	//int deltaX = x - lastX;
	//int deltaY = y - lastY;
	//float xOffset = sensitivity * deltaX;
	//float yOffset = sensitivity * deltaY;

	float xOffset = static_cast<float>(x - lastX);
	float yOffset = static_cast<float>(lastY - y);

	//rotate
	if (mouse == Mouse::Left && mouseState == MouseState::Down)
	{
		camera->ProcessMouseMovement(xOffset, yOffset);
	}
	//Zoom
	else if (mouse == Mouse::Right && mouseState == MouseState::Down)
	{
		camera->ProcessMouseScroll(yOffset*0.5);
	}

	SetDrawFlag();

	lastX = x;
	lastY = y;
}

void RenderPluginBase::Reshape(GLsizei w, GLsizei h)
{
	windowWidth = w;
	windowHeight = h;
	SetDrawFlag();
}

void RenderPluginBase::InitializeMatrix()
{
	modelMat = glm::mat4();
	modelMat = glm::rotate(modelMat, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//projectMat = glm::perspective(45.0f, 8.0f / 6.0f, 0.1f, 100.0f);

	//modelMat = glm::mat4();
}

void RenderPluginBase::SetMatrix()
{
	GLint modelLoc = glGetUniformLocation(currentProgram, "modelMat");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
	GLint viewLoc = glGetUniformLocation(currentProgram, "viewMat");
	viewMat = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	GLint projectLoc = glGetUniformLocation(currentProgram, "projectMat");
	projectMat = camera->GetProjectMatrix();
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projectMat));
}

void RenderPluginBase::Set2DTexture(GLuint * texture, const char * fileName)
{
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//加载并生成纹理 
	int width, height;
	unsigned char * image = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderPluginBase::SetDrawFlag()
{
	drawFlag = true;
}