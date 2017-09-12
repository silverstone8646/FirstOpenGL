#version 450

//in vec4 ourColor;
in vec2 ourTexCoord;
out vec4 color;

uniform sampler2D wallTexture;
uniform sampler2D faceTexture;

void main()
{
    //color = ourColor;

	vec2 texCoord = vec2(ourTexCoord.x, 1 - ourTexCoord.y);
	color = mix(texture(wallTexture, texCoord), texture(faceTexture, texCoord), 0.2);
}