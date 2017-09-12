#include "Mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>


Mesh::Mesh(const std::vector<Vertex>& vertices, 
	const std::vector<GLuint>& indices, 
	const std::vector<Texture>& textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	// set the vertex buffers and its attribute pointers.
	this->Initialize_i();
}

void Mesh::Draw(GLuint program)
{
	GLuint diffuseNr = 1; //漫反射纹理起始值
	GLuint specularNr = 1; //镜面纹理起始值

	for (GLuint i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);

		//get N in texture_diffuseN or texture_specularN
		std::stringstream ss;
		std::string name = this->textures[i].type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; 
		else if (name == "texture_specular")
			ss << specularNr++; 
		std::string number = ss.str();

		GLuint textureLoc = glGetUniformLocation(program, (name+number).c_str());
		glUniform1i(textureLoc, i);
	}

	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//unbind after draw
	//for (GLuint i = 0; i < textures.size(); i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i);
	//}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::Initialize_i()
{
	//create buffers and arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	// vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// indices data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//set vertex attribute pointer
	//vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLuint*)0);
	//vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLuint*)offsetof(Vertex, normal));
	//vertex texCoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLuint*)offsetof(Vertex, texCoords));

	//unbind VAO
	glBindVertexArray(0);
}

