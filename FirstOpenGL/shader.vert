#version 450
layout (location = 0) in vec3 position; 
//layout (location = 1) in vec3 color;    
layout(location = 1) in vec2 TexCoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectMat;

//out vec4 ourColor;
out vec2 ourTexCoord;

void main()
{
    //gl_Position = vec4(position, 1.0); 
    //ourColor = vec4(0.5f, 0.0f, 0.0f, 1.0f); 

	gl_Position = projectMat * viewMat * modelMat * vec4(position, 1.0);
	ourTexCoord = TexCoord;
}