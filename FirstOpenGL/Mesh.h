#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glew.h>
#include "glm.hpp"

#include <assimp/types.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	GLuint id;
	std::string type;
	aiString path; //store the path of texture to compare with other texture
};

class Mesh
{
public:
	/*  Functions  */
	// Constructor
	Mesh(const std::vector<Vertex>& vertices, 
		const std::vector<GLuint>& indices, 
		const std::vector<Texture>& textures);

	void Draw(GLuint program);

private:
	//mesh data
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	//render data
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	//initialize
	void Initialize_i();
};

#endif // !MESH_H

