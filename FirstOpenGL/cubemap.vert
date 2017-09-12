#version 450 

layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projectMat;
uniform mat4 viewMat;

void main()
{
	gl_Position = projectMat * viewMat * vec4(position, 1.0f);
	TexCoords = position;
}