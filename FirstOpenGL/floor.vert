#version 450 core
layout (location = 0) in vec3 position; 
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 fragPos;
out vec3 myNormal;
out vec2 TexCoords;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectMat;

void main()
{
    gl_Position = projectMat * viewMat * modelMat * vec4(position, 1.0f);

	vec4 temp = transpose(inverse(modelMat)) * vec4(normal, 1.0f);
	myNormal = vec3(temp);

    TexCoords = texCoords;

	fragPos = vec3(modelMat * vec4(position, 1.0f));
}