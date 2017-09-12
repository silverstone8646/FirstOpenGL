#ifndef CUBEMAP_MESH_RENDER_PLUGIN_H
#define CUBEMAP_MESH_RENDER_PLUGIN_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "RenderPluginBase.h"

class CubeMapMeshRenderPlugin : public RenderPluginBase
{
public:
	CubeMapMeshRenderPlugin();
	void Initialize(const std::string& path);

	virtual void Render();
	virtual void PreRender();
	virtual void Keyboard(unsigned char key, int x, int y);

	virtual void InitializeMatrix();

private:
	void LoadModels_i(const std::string& path);
	void ProcessNode_i(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh_i(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures_i(aiMaterial* mat,
		aiTextureType type, const std::string& typeName);

	void DrawCubeMap_i();
	void DrawMeshes_i();

	void PreRender_Cubemap_i();
	void SetCubeMapTexture_i();

	void SetRenderMode_i();

private:
	glm::vec3 lightPos;//相机坐标系下

	std::vector<Mesh> meshes;
	std::vector<Texture> loadedTextures;
	std::string directory;

	GLuint cubemapVAO;
	GLuint cubemapVBO;

	GLuint cubemapProgram;
	GLuint cubemapTexture;

	int renderMode; //1 for normal, 2 for metal, 3 for transparent
};

#endif // !MESH_RENDER_PLUGIN_H

