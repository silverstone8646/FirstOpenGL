#include "Camera.h"

float rotateSensitivity = 0.05f;
float zoomSensitivity = 0.01f;

Camera::Camera(glm::vec3 position_, glm::vec3 up_)
	: position(position_), worldUp(up_), 
	yaw(-90.0f), pitch(0.0f), worldCenterToCamera(3.0f), fov(45.0f)
{
	UpdateCameraPara_i();
}

glm::mat4 Camera::GetViewMatrix()
{
	//return glm::lookAt(position, position + worldCenterToCamera * front, cameraUp);
	return glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
}

glm::mat4 Camera::GetProjectMatrix()
{
	return glm::perspective(fov, 8.0f / 6.0f, 0.1f, 100.0f);
}

glm::vec3 Camera::GetCameraPosition()
{
	return position;
}

//rotate
void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
	xOffset *= rotateSensitivity;
	yOffset *= rotateSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	UpdateCameraPara_i();
}

//zoom
void Camera::ProcessMouseScroll(float yOffset)
{
	worldCenterToCamera += yOffset * zoomSensitivity;
	
	//if (this->fov >= 1.0f && this->fov <= 45.0f)
	//	this->fov -= yOffset;
	//if (this->fov <= 1.0f)
	//	this->fov = 1.0f;
	//if (this->fov >= 45.0f)
	//	this->fov = 45.0f;

	UpdateCameraPara_i();
}

void Camera::SetCamera(glm::vec3 pos, glm::vec3 up)
{
	position = pos;
	cameraUp = up;

	//recalculate yaw, pitch and distance to world center.
	worldCenterToCamera = glm::length(position);
}

void Camera::UpdateCameraPara_i()
{
	//相机view对着world中心
	glm::vec3 frontTemp;
	frontTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontTemp.y = sin(glm::radians(pitch));
	frontTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	//相机view方向
	front = glm::normalize(frontTemp);
	right = glm::normalize(glm::cross(front, worldUp));
	cameraUp = glm::normalize(glm::cross(right, front));

	//相机位置
	position = -glm::vec3(front.x * worldCenterToCamera, 
		front.y * worldCenterToCamera, 
		front.z *worldCenterToCamera);
}
