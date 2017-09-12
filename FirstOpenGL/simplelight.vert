#version 450
layout (location = 0) in vec3 position; 
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT
{
	vec3 myNormal;
	vec3 fragPos;
	vec2 TexCoords;
	vec4 fragPosInLightSpace;
}vs_out;

//out vec3 myNormal;
//out vec3 fragPos;
//out vec2 TexCoords;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectMat;
uniform mat4 lightSpaceMat;

void main()
{
	gl_Position = projectMat * viewMat * modelMat * vec4(position, 1.0);
	
	//myNormal = normal;
	//myNormal = mat3(transpose(inverse(modelMat))) * normal;
	vec4 temp = transpose(inverse(modelMat)) * vec4(normal, 1.0f);
	vs_out.myNormal = vec3(temp);

	vs_out.TexCoords = texCoords;
	
	vs_out.fragPos = vec3(modelMat * vec4(position, 1.0f));

	vs_out.fragPosInLightSpace = lightSpaceMat * vec4(vs_out.fragPos, 1.0f);
}