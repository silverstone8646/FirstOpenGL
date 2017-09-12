#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 myNormal;
out vec3 fragPos;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectMat;

void main()
{
	gl_Position = projectMat * viewMat * modelMat * vec4(position, 1.0f);

	vec4 temp = transpose(inverse(modelMat)) * vec4(normal, 1.0f);
	myNormal = vec3(temp);

	fragPos = vec3(modelMat * vec4(position, 1.0f));
	TexCoords = texCoords;
}