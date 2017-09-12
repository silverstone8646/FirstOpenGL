#include "LightRenderPlugin.h"
#include "Program.h"
#include "Sphere.h"

#include "SOIL.h"

#include <iostream>

namespace
{
	// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
	// and post-processing effects.
	GLuint quadVAO = 0;
	GLuint quadVBO;
	void RenderQuad()
	{
		if (quadVAO == 0)
		{
			GLfloat quadVertices[] = {
				// Positions        // Texture Coords
				-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
				1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
				1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			};
			// Setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}

glm::vec3 lightPos(2.5f, 2.5f, 0.0f);//世界坐标系下
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

const float rotateSensitivity = 0.05f;
const float PI = 3.1415926;

LightRenderPlugin::LightRenderPlugin()
	: operationMode(CAMERA)
{
	m_spSphere = std::make_shared<Sphere>();
	m_spSphere->SetPitchStep(30);
	m_spSphere->SetYawStep(30);
	m_spSphere->SetRadius(0.8);
}

void LightRenderPlugin::Render()
{
	if (!drawFlag)
	{
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//1. draw shadow
	RenderShadow();

	//2. draw other things
	RenderObjects();

	drawFlag = false;
}

void LightRenderPlugin::PreRender()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//物体 
	Program Program1(ksSimpleLightVertexShaderFile, ksSimpleLightFragmentShaderFile);
	objProgram = Program1.program;
	currentProgram = objProgram; //物体可以通过基类来操作
	
	//灯
	Program Program2(ksSimpleLightVertexShaderFile, ksSimpleLampFragmentShaderFile);
	lampProgram = Program2.program;

	//地板
	Program Program3(ksFloorVertexShaderFile, ksFloorFragmentShaderFile);
	floorProgram = Program3.program;

	//under model coordinate
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	GLfloat planeVertices[] = {
		// Positions		  //normal			// Texture Coords 
		5.0f,  -1.0f,  5.0f,  0.0f, 1.0f, 0.0f,  3.0f, 0.0f,
		-5.0f, -1.0f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-5.0f, -1.0f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 3.0f,

		5.0f,  -1.0f,  5.0f,  0.0f, 1.0f, 0.0f,  3.0f, 0.0f,
		-5.0f, -1.0f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 3.0f,
		5.0f,  -1.0f, -5.0f,  0.0f, 1.0f, 0.0f,  3.0f, 3.0f
	};

	///////////////////
	//cubic object
	//create containerVAO
	glGenVertexArrays(1, &containerVAO);
	glBindVertexArray(containerVAO);

	//create VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);

	//copy vertex data into OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//normal attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)*3));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//uniform matrix
	InitializeMatrix();

	///////////////////////
	//earth obj
	//create earth VAO
	std::vector<GLfloat> vecVertex;
	std::vector<GLuint> vecVertexIndices;
	m_spSphere->GetVertexArray(vecVertex);
	m_spSphere->GetVertexIndices(vecVertexIndices);
	GLfloat* earthVertices = vecVertex.data();
	GLuint* earthVertexIndices = vecVertexIndices.data();
	std::cout << "size of vertices is: " << vecVertex.size();
	glGenVertexArrays(1, &earthVAO);
	glBindVertexArray(earthVAO);
	GLuint earthVBO;
	glGenBuffers(1, &earthVBO);
	glBindBuffer(GL_ARRAY_BUFFER, earthVBO);
	glBufferData(GL_ARRAY_BUFFER, vecVertex.size()*sizeof(GLfloat), earthVertices, GL_STATIC_DRAW);

	GLuint earthEBO;
	glGenBuffers(1, &earthEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, earthEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecVertexIndices.size() * sizeof(GLuint), earthVertexIndices, GL_STATIC_DRAW);

	//point
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLfloat*)0);
	glEnableVertexAttribArray(0);
	//normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLfloat*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	//texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLfloat*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	InitializeEarthMatrix();

	//earth texture
	Set2DTexture(&earthTexture, "earth.jpg");

	///////////////////////
	//light obj
	//create light VAO
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	InitializeLampMatrix();

	////////////////////////
	//floor
	//create floor VAO
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);
	//create VBO
	GLuint floorVBO;
	glGenBuffers(1, &floorVBO);
	//copy vertex data into OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//floor texture
	Set2DTexture(&floorTexture, "floor.jpg");

	////////////////////////
	//shadow
	InitializeShadowResource();

	////////////////////////
	//debub shadow program
	Program Program(ksDebugDepthVertexShaderFile, ksDebugDepthFragmentShaderFile);
	debugDepthProgram = Program.program;
}

void LightRenderPlugin::Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'e':
		operationMode = EARTH;
		break;
	case 'c':
		operationMode = CAMERA;
		break;
	case 'b':
		operationMode = BOX;
		break;
	default:
		break;
	}
}

void LightRenderPlugin::MouseMotion(int x, int y)
{
	if (operationMode == CAMERA)
	{
		RenderPluginBase::MouseMotion(x, y);
	}
	else if (operationMode == EARTH)
	{
		RotateEarth(x, y);
	}
	else if (operationMode == BOX)
	{
		RotateBox(x, y);
	}
}

void LightRenderPlugin::SetMatrix()
{
	RenderPluginBase::SetMatrix();

	GLint lightSpaceMatLoc = glGetUniformLocation(objProgram, "lightSpaceMat");
	glUniformMatrix4fv(lightSpaceMatLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMat));
}

void LightRenderPlugin::InitializeLampMatrix()
{
	//viewModel和projectModel均使用和物体相同的
	lampModelMat = glm::mat4();
	lampModelMat = glm::translate(lampModelMat, lightPos);
	lampModelMat = glm::scale(lampModelMat, glm::vec3(0.1f));
}

void LightRenderPlugin::InitializeEarthMatrix()
{
	//viewModel和projectModel均使用和物体相同的
	earthModelMat = glm::mat4();
	earthModelMat = glm::translate(earthModelMat, glm::vec3(-1.0, 0.0, -2.0));
	initialEarthMat = earthModelMat;
}

void LightRenderPlugin::SetLampMatrix()
{
	//lamp和obj使用同一个vertex shader
	GLint lampModelLoc = glGetUniformLocation(lampProgram, "modelMat"); 
	glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(lampModelMat));
	GLint viewLoc = glGetUniformLocation(lampProgram, "viewMat");
	viewMat = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	GLint projectLoc = glGetUniformLocation(lampProgram, "projectMat");
	projectMat = camera->GetProjectMatrix();
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projectMat));
}

void LightRenderPlugin::SetEarthMatrix()
{
	//和box不同的仅为modelMat
	GLint modelLoc = glGetUniformLocation(objProgram, "modelMat");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(earthModelMat));
	GLint viewLoc = glGetUniformLocation(objProgram, "viewMat");
	viewMat = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	GLint projectLoc = glGetUniformLocation(objProgram, "projectMat");
	projectMat = camera->GetProjectMatrix();
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projectMat));
}

void LightRenderPlugin::SetFloorMatrix()
{
	GLint modelLoc = glGetUniformLocation(floorProgram, "modelMat");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
	GLint viewLoc = glGetUniformLocation(floorProgram, "viewMat");
	viewMat = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	GLint projectLoc = glGetUniformLocation(floorProgram, "projectMat");
	projectMat = camera->GetProjectMatrix();
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projectMat));
}

//void LightRenderPlugin::Set2DTexture(GLuint * texture, const char * fileName)
//{
//	glGenTextures(1, texture);
//	glBindTexture(GL_TEXTURE_2D, *texture);
//
//	//设置纹理过滤方式
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	//加载并生成纹理 
//	int width, height;
//	unsigned char * image = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGB);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//	glGenerateMipmap(GL_TEXTURE_2D);
//	SOIL_free_image_data(image);
//	glBindTexture(GL_TEXTURE_2D, 0);
//}

void LightRenderPlugin::SetUniformValue(GLuint program)
{
	GLint lightColorLoc = glGetUniformLocation(program, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	glm::vec3 cameraPos = camera->GetCameraPosition();
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	GLint matAmbientLoc = glGetUniformLocation(program, "material.ambient");
	GLint matDiffuselLoc = glGetUniformLocation(program, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(program, "material.specular");
	GLint matShininessLoc = glGetUniformLocation(program, "material.shininess");
	glUniform3f(matAmbientLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(matDiffuselLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(matSpecularLoc, 1.0f, 1.0f, 1.0f);
	glUniform1f(matShininessLoc, 64.0f);

	GLint lightPosLoc = glGetUniformLocation(program, "light.position");
	GLint lightAmbientLoc = glGetUniformLocation(program, "light.ambient");
	GLint lightDiffuselLoc = glGetUniformLocation(program, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(program, "light.specular");

	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(lightAmbientLoc, 0.2f);
	glUniform1f(lightDiffuselLoc, 0.6f);
	glUniform1f(lightSpecularLoc, 1.0f);
}

void LightRenderPlugin::SetEarthUniformValue(GLuint program)
{
	GLint lightColorLoc = glGetUniformLocation(program, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	glm::vec3 cameraPos = camera->GetCameraPosition();
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	GLint matAmbientLoc = glGetUniformLocation(program, "material.ambient");
	GLint matDiffuselLoc = glGetUniformLocation(program, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(program, "material.specular");
	GLint matShininessLoc = glGetUniformLocation(program, "material.shininess");
	glUniform3f(matAmbientLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(matDiffuselLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(matSpecularLoc, 1.0f, 1.0f, 1.0f);
	glUniform1f(matShininessLoc, 64.0f);

	GLint lightPosLoc = glGetUniformLocation(program, "light.position");
	GLint lightAmbientLoc = glGetUniformLocation(program, "light.ambient");
	GLint lightDiffuselLoc = glGetUniformLocation(program, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(program, "light.specular");

	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(lightAmbientLoc, 0.3f);
	glUniform1f(lightDiffuselLoc, 0.6f);
	glUniform1f(lightSpecularLoc, 0.4f);
}

void LightRenderPlugin::InitializeShadowResource()
{
	Program program(ksSimpleDepthVertexShaderFile, ksSimpleDepthFragmentShaderFile);
	shadowProgram = program.program;

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//nothing to do with color buffer.
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightRenderPlugin::RenderObjects()
{
	glViewport(0, 0, windowWidth, windowHeight);

	//1. draw floor(floor,box and earth use the same shaders)
	//
	glUseProgram(objProgram);
	SetMatrix();
	SetUniformValue(objProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glUniform1i(glGetUniformLocation(objProgram, "objTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(objProgram, "shadowMap"), 1);

	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	//2. Draw box
	//
	glBindVertexArray(containerVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	//3. draw earth
	//
	SetEarthMatrix();
	SetEarthUniformValue(objProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);
	glUniform1i(glGetUniformLocation(objProgram, "objTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(objProgram, "shadowMap"), 1);

	glBindVertexArray(earthVAO);
	int vertexCount = m_spSphere->GetTriangleCount() * 3;
	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//4. draw cubic light
	//
	glUseProgram(lampProgram);
	SetLampMatrix();
	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void LightRenderPlugin::RenderShadow()
{
	glUseProgram(shadowProgram);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glCullFace(GL_FRONT);
	
	SetShadowMatrix(modelMat);

	//1. draw floor
	//
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glUniform1i(glGetUniformLocation(floorProgram, "floorTexture"), 0);
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	//2. Draw object
	//
	glBindVertexArray(containerVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	//3. draw cubic light
	//
	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	//4. Draw earth
	SetShadowMatrix(earthModelMat);
	glBindVertexArray(earthVAO);
	glDrawElements(GL_TRIANGLES, m_spSphere->GetTriangleCount() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//recover to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);

	//for test
	//render our frame buffer to screen.
	//glViewport(0, 0, windowWidth, windowHeight);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(debugDepthProgram);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//glUniform1i(glGetUniformLocation(debugDepthProgram, "depthMap"), 0);
	//RenderQuad();
}

void LightRenderPlugin::SetShadowMatrix(const glm::mat4& inputModelMat)
{
	GLint modelLoc = glGetUniformLocation(shadowProgram, "modelMat");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(inputModelMat));
	GLint lightSpaceLoc = glGetUniformLocation(shadowProgram, "lightSpaceMat");
	//相对于普通的V-P矩阵，阴影对应的V-P矩阵通过光源位置和世界坐标的中心决定
	GLfloat near_plane = 0.1f, far_plane = 15.0f;

	glm::mat4 lightProjectMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightViewMat = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(1.0));
	lightSpaceMat = lightProjectMat * lightViewMat;

	glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMat));
}

void LightRenderPlugin::RotateEarth(int x, int y)
{
	float sensitivity = 0.05f;
	float xOffset = static_cast<float>(x - lastX);
	float yOffset = static_cast<float>(lastY - y);

	//rotate
	if (mouse == Mouse::Left && mouseState == MouseState::Down)
	{
		float tempYaw = xOffset * rotateSensitivity * 180/PI;
		float tempPitch =  yOffset * rotateSensitivity * 180/PI;

		glm::mat4 viewMat = camera->GetViewMatrix();
		glm::mat4 inverseViewMat = glm::inverse(viewMat);

		//世界坐标系下的旋转矩阵
		glm::mat4 rotateMat;
		rotateMat = glm::rotate(rotateMat, tempYaw, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 secondAxis = glm::vec3(-sin(tempYaw), cos(tempYaw), 0.0);
		rotateMat = glm::rotate(rotateMat, tempPitch, secondAxis);

		rotateMat = rotateMat * viewMat;

		//改变earth的模型矩阵
		glm::mat4 inverseInitialEarthModelMat = glm::inverse(initialEarthMat);
		//glm::mat4 rotateMatInModelCoord = rotateMat * inverseEarthModelMat;
		earthModelMat = initialEarthMat * rotateMat * inverseInitialEarthModelMat * earthModelMat;
	}
	////Zoom
	//else if (mouse == Mouse::Right && mouseState == MouseState::Down)
	//{
	//	camera->ProcessMouseScroll(yOffset*0.5);
	//}

	SetDrawFlag();

	lastX = x;
	lastY = y;
}

void LightRenderPlugin::RotateBox(int x, int y)
{

}