#include "MeshRenderPlugin.h"

#include <iostream>
#include <string>
#include "SOIL.h"

#include "defines.h"
#include "Program.h"
#include "Mesh.h"

MeshRenderPlugin::MeshRenderPlugin()
{
	lightPos = glm::vec3(4.0f, 4.0f, 0.0f);

	camera->SetCamera(glm::vec3(0.0f, 4.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void MeshRenderPlugin::Initialize(const std::string & path)
{
	this->LoadModels_i(path);
}

void MeshRenderPlugin::Render()
{
	if (!drawFlag)
	{
		return;
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, windowWidth, windowHeight);

	//draw floor
	DrawFloor_i();

	//draw meshes
	//DrawMeshes_i();

	drawFlag = false;
}

void MeshRenderPlugin::PreRender()
{
	glEnable(GL_DEPTH_TEST);

	//this->Initialize(ksMeshModelFile);
	this->Initialize(ksChahuModelFile);

	/////////////////////////////////////////////////////////////////////////
	//floor program
	Program ProgramFloor(ksSimpleLightVertexShaderFile, ksSimpleLightFragmentShaderFile);
	floorProgram = ProgramFloor.program;

	GLfloat planeVertices[] = {
		// Positions		  //normal			// Texture Coords 
		5.0f,  -1.0f,  5.0f,  0.0f, 1.0f, 0.0f,  3.0f, 0.0f,
		-5.0f, -1.0f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-5.0f, -1.0f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 3.0f,

		5.0f,  -1.0f,  5.0f,  0.0f, 1.0f, 0.0f,  3.0f, 0.0f,
		-5.0f, -1.0f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 3.0f,
		5.0f,  -1.0f, -5.0f,  0.0f, 1.0f, 0.0f,  3.0f, 3.0f
	};

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
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//floor texture
	Set2DTexture(&floorTexture, "floor.jpg");

	////////////////////////////////////////////////////////////////////////////
	//mesh program
	Program program(ksSimpleLightVertexShaderFile, ksDepthPeelingFragmentShaderFile);
	currentProgram = program.program;
	program.Use();

	//set uniform
	GLint lightColorLoc = glGetUniformLocation(currentProgram, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

	GLint lightPosLoc = glGetUniformLocation(currentProgram, "light.position");
	GLint lightAmbientLoc = glGetUniformLocation(currentProgram, "light.ambient");
	GLint lightDiffuselLoc = glGetUniformLocation(currentProgram, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(currentProgram, "light.specular");

	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(lightAmbientLoc, 0.1f);
	glUniform1f(lightDiffuselLoc, 0.7f);
	glUniform1f(lightSpecularLoc, 0.6f);

	// transform matrix
	InitializeMatrix();
}

void MeshRenderPlugin::Keyboard(unsigned char key, int x, int y)
{
	key, x, y;
}

void MeshRenderPlugin::InitializeMatrix()
{
	RenderPluginBase::InitializeMatrix();

	meshModelMat = glm::mat4();
	// It's a bit too big for our scene, so scale it down
	meshModelMat = glm::scale(meshModelMat, glm::vec3(0.04f, 0.04f, 0.04f));
}

void MeshRenderPlugin::LoadModels_i(const std::string & path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	this->directory = path.substr(0, path.find_last_of('/'));

	this->ProcessNode_i(scene->mRootNode, scene);
}

void MeshRenderPlugin::ProcessNode_i(aiNode * node, const aiScene * scene)
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		//node中只是存的mesh的引用，真正的mesh数据在Scene中
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(ProcessMesh_i(mesh, scene));
	}

	//递归处理子节点
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode_i(node->mChildren[i], scene);
	}
}

Mesh MeshRenderPlugin::ProcessMesh_i(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	//处理顶点数据（坐标，法向量，纹理坐标）
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		// Normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		else
		{
			GLuint iLast = i > 0 ? i - 1 : mesh->mVertices->Length() - 1;
			GLuint iNext = i < mesh->mVertices->Length() - 1 ? i + 1 : 0;

			auto v1 = glm::normalize(glm::vec3(mesh->mVertices[iLast].x - mesh->mVertices[i].x,
				mesh->mVertices[iLast].y - mesh->mVertices[i].y,
				mesh->mVertices[iLast].z - mesh->mVertices[i].z));
			auto v2 = glm::normalize(glm::vec3(mesh->mVertices[iNext].x - mesh->mVertices[i].x,
				mesh->mVertices[iNext].y - mesh->mVertices[i].y,
				mesh->mVertices[iNext].z - mesh->mVertices[i].z));
			auto v = glm::cross(v2, v1);

			vertex.normal = v;
		}
		//texture coords
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	//处理顶点索引
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// pure mesh, without texture
	////处理材质
	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	//	// texture name format in shader:
	//	// Diffuse: texture_diffuseN
	//	// Specular: texture_specularN
	//	// Normal: texture_normalN

	//	// 1. Diffuse maps
	//	std::vector<Texture> diffuseMaps = this->LoadMaterialTextures_i(
	//		material, aiTextureType_DIFFUSE, "texture_diffuse");
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//	// 2. Specular maps
	//	std::vector<Texture> specularMaps = this->LoadMaterialTextures_i(
	//		material, aiTextureType_SPECULAR, "texture_specular");
	//	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//}

	return std::move(Mesh(vertices, indices, textures));
}

void MeshRenderPlugin::DrawFloor_i()
{
	glUseProgram(floorProgram);
	SetMatrix();
	SetUniformValue(floorProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glUniform1i(glGetUniformLocation(floorProgram, "objTexture"), 0);

	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


void MeshRenderPlugin::DrawMeshes_i()
{
	glUseProgram(currentProgram);

	SetMeshMatrix();

	SetUniformValue(currentProgram);

	//set uniform may change every path
	GLint viewPosLoc = glGetUniformLocation(currentProgram, "viewPos");
	glm::vec3 cameraPos = camera->GetCameraPosition();
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	GLint lightPosLoc = glGetUniformLocation(currentProgram, "lightPos");
	glm::mat4 viewMat = camera->GetViewMatrix();
	glm::mat4 viewInverseMat = glm::inverse(viewMat);
	glm::vec3 lightPosInWorld = glm::vec3(viewInverseMat * glm::vec4(lightPos, 1.0));
	glUniform3f(lightPosLoc, lightPosInWorld.x, lightPosInWorld.y, lightPosInWorld.z);

	SetRenderMode_i();

	for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
	{
		itr->Draw(currentProgram);
	}
}

void MeshRenderPlugin::SetUniformValue(GLuint program)
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

void MeshRenderPlugin::SetMeshMatrix()
{
	GLint modelLoc = glGetUniformLocation(currentProgram, "modelMat");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(meshModelMat));
	GLint viewLoc = glGetUniformLocation(currentProgram, "viewMat");
	viewMat = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	GLint projectLoc = glGetUniformLocation(currentProgram, "projectMat");
	projectMat = camera->GetProjectMatrix();
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projectMat));
}

void MeshRenderPlugin::SetRenderMode_i()
{
	GLint renderModeLoc = glGetUniformLocation(currentProgram, "renderMode");
	glUniform1i(renderModeLoc, renderMode);

	SetDrawFlag();
}
