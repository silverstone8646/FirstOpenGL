#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include <glew.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), 
		   glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectMatrix();
	glm::vec3 GetCameraPosition();

	void ProcessMouseMovement(float xOffset, float yOffset);
	void ProcessMouseScroll(float yOffset);

	void SetCamera(glm::vec3 pos, glm::vec3 up);

private:
	void UpdateCameraPara_i();

private:
	//position(under world coordinate)
	glm::vec3 position; //camera position
	glm::vec3 front;
	glm::vec3 cameraUp;
	glm::vec3 worldUp;
	glm::vec3 right;

	//direction
	float yaw; //航偏角
	float pitch; //俯仰角
	float worldCenterToCamera;//世界坐标中心到相机的距离
	
	//fov
	float fov;

    //matrix
	//glm::mat4 modelMat;
	//glm::mat4 viewMat;
	//glm::mat4 projectMat;
};

#endif // !CAMERA_H
