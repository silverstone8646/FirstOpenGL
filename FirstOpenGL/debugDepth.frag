#version 450 core

out vec4 color;
in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
	float depth = texture(depthMap, TexCoords).r;
	color = vec4(vec3(depth), 1.0);
}