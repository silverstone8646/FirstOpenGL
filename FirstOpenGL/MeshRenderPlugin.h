
#ifndef MESH_RENDER_PLUGIN_H
#define MESH_RENDER_PLUGIN_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "RenderPluginBase.h"

class MeshRenderPlugin : public RenderPluginBase
{
public:
	MeshRenderPlugin();
	void Initialize(const std::string& path);

	virtual void Render();
	virtual void PreRender();
	virtual void Keyboard(unsigned char key, int x, int y);

	virtual void InitializeMatrix();

private:
	void LoadModels_i(const std::string& path);
	void ProcessNode_i(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh_i(aiMesh* mesh, const aiScene* scene);

	void DrawFloor_i();

	void DrawMeshes_i();

	void SetRenderMode_i();

	void SetUniformValue(GLuint program);

	void SetMeshMatrix();

private:
	glm::vec3 lightPos;//相机坐标系下

	std::vector<Mesh> meshes;
	std::string directory;

	glm::mat4 meshModelMat;
	
	GLuint floorVAO;
	GLuint floorTexture;
	GLuint floorProgram;

	int renderMode; //1 for normal, 2 for metal, 3 for transparent
};

#endif // !MESH_RENDER_PLUGIN_H

