#include "CubeMapMeshRenderPlugin.h"

#include <iostream>
#include <string>
#include "SOIL.h"

#include "defines.h"
#include "Program.h"
#include "Mesh.h"

const std::string ksRight = "../skybox/right.jpg";
const std::string ksLeft = "../skybox/left.jpg";
const std::string ksTop = "../skybox/top.jpg";
const std::string ksBottom = "../skybox/bottom.jpg";
const std::string ksBack = "../skybox/back.jpg";
const std::string ksFront = "../skybox/front.jpg";

namespace
{
	GLint TextureFromFile(const char* path, std::string directory)
	{
		//Generate texture ID and load texture data 
		std::string filename = std::string(path);
		filename = directory + '/' + filename;
		GLuint textureID;
		glGenTextures(1, &textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);
		return textureID;
	}
}

CubeMapMeshRenderPlugin::CubeMapMeshRenderPlugin() : renderMode(1)
{
	lightPos = glm::vec3(2.0f, 2.0f, 0.0f);
}

void CubeMapMeshRenderPlugin::Initialize(const std::string& path)
{
	this->LoadModels_i(path);
}


void CubeMapMeshRenderPlugin::Render()
{
	if (!drawFlag)
	{
		return;
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, windowWidth, windowHeight);

	//draw skybox
	DrawCubeMap_i();

	//draw meshes
	DrawMeshes_i();

	drawFlag = false;
}

void CubeMapMeshRenderPlugin::PreRender()
{
	glEnable(GL_DEPTH_TEST);

	//this->Initialize(ksMeshModelFile);
	this->Initialize(ksChahuModelFile);

	Program skyboxProgram(ksSkyboxVertexShaderFile, ksSkyboxFragmentShaderFile);
	cubemapProgram = skyboxProgram.program;
	skyboxProgram.Use();

	PreRender_Cubemap_i();

	Program program(ksMeshModelVertexShaderFile, ksMeshModelFragmentShaderFile);
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

	//set cube map texture
	SetCubeMapTexture_i();

	// transform matrix
	InitializeMatrix();
}

void CubeMapMeshRenderPlugin::Keyboard(unsigned char key, int x, int y)
{
	key, x, y;
	//switch (key)
	//{
	//case 'n': //normal
	//	renderMode = 1;
	//	SetRenderMode_i();
	//	break;
	//case 'g': //glass
	//	renderMode = 2;
	//	SetRenderMode_i();
	//	break;
	//case 'm': //metal
	//	renderMode = 3;
	//	SetRenderMode_i();
	//	break;
	//default:
	//	break;
	//}
}

void CubeMapMeshRenderPlugin::InitializeMatrix()
{
	modelMat = glm::mat4();
	// Translate it down a bit so it's at the center of the scene
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, -1.75f, 0.0f));
	// It's a bit too big for our scene, so scale it down
	modelMat = glm::scale(modelMat, glm::vec3(0.1f, 0.1f, 0.1f));
}

void CubeMapMeshRenderPlugin::LoadModels_i(const std::string& path)
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

void CubeMapMeshRenderPlugin::ProcessNode_i(aiNode * node, const aiScene * scene)
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

Mesh CubeMapMeshRenderPlugin::ProcessMesh_i(aiMesh * mesh, const aiScene * scene)
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

	//处理材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// texture name format in shader:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		std::vector<Texture> diffuseMaps = this->LoadMaterialTextures_i(
			material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		std::vector<Texture> specularMaps = this->LoadMaterialTextures_i(
			material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return std::move(Mesh(vertices, indices, textures));
}

std::vector<Texture> CubeMapMeshRenderPlugin::LoadMaterialTextures_i(
	aiMaterial * material, aiTextureType type, const std::string & typeName)
{
	std::vector<Texture> textures;
	for (GLuint i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		
		// Check if texture was loaded before and if so, 
		// continue to next iteration: skip loading a new texture

		bool bSkip = false;
		for (int i = 0; i < loadedTextures.size(); i++)
		{
			if (loadedTextures[i].path == str)
			{
				textures.push_back(loadedTextures[i]);
				bSkip = true;
				break;
			}
		}

		if (!bSkip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			loadedTextures.push_back(texture);
		}
		
	}
	return textures;
}

void CubeMapMeshRenderPlugin::DrawCubeMap_i()
{
	glUseProgram(cubemapProgram);

	glDepthMask(GL_FALSE);

	glm::mat4 viewMat = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	glm::mat4 projectMat = camera->GetProjectMatrix();
	glUniformMatrix4fv(glGetUniformLocation(cubemapProgram, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(glGetUniformLocation(cubemapProgram, "projectMat"), 1, GL_FALSE, glm::value_ptr(projectMat));

	glBindVertexArray(cubemapVAO);
	glUniform1i(glGetUniformLocation(cubemapProgram, "skybox"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
}

void CubeMapMeshRenderPlugin::DrawMeshes_i()
{
	glUseProgram(currentProgram);

	SetMatrix();

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

	//还有texture_diffuse1和texture_specular1两个纹理
	glUniform1i(glGetUniformLocation(cubemapProgram, "skybox"), 2);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
	{
		itr->Draw(currentProgram);
	}
}

void CubeMapMeshRenderPlugin::PreRender_Cubemap_i()
{
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &cubemapVAO);
	glGenBuffers(1, &cubemapVBO);

	glBindVertexArray(cubemapVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void CubeMapMeshRenderPlugin::SetCubeMapTexture_i()
{
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	std::vector<std::string> vecfaces;
	vecfaces.push_back(ksRight);
	vecfaces.push_back(ksLeft);
	vecfaces.push_back(ksTop);
	vecfaces.push_back(ksBottom);
	vecfaces.push_back(ksBack);
	vecfaces.push_back(ksFront);
	
	int width, height;
	unsigned char* image;

	for (int i = 0; i < vecfaces.size(); i++)
	{
		image = SOIL_load_image(vecfaces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMapMeshRenderPlugin::SetRenderMode_i()
{
	GLint renderModeLoc = glGetUniformLocation(currentProgram, "renderMode");
	glUniform1i(renderModeLoc, renderMode);

	SetDrawFlag();
}


