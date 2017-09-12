#version 450 core

in vec2 TexCoords;
out vec4 color;

in vec3 myNormal;
in vec3 fragPos;

uniform vec3 lightColor;
uniform vec3 viewPos;

//material中的系数是不同光线的颜色
//Light中的系数则是不同光线最后组合在一起需要乘以的系数
struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light
{
	vec3 position;
	float ambient; 
	float diffuse;
	float specular;
};
uniform Light light;

uniform sampler2D floorTexture;

void main()
{             
	vec3 normal = normalize(myNormal);
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	//ambient
	vec3 ambient = material.ambient * lightColor;

	//diffuse
	float diffuseFactor = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = material.diffuse * vec3(texture(floorTexture, TexCoords)) * lightColor;

	//specular
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = material.specular * specFactor * lightColor;

	vec3 result = (light.ambient * ambient + light.diffuse * diffuse + light.specular * specular);
	color = vec4(result, 1.0f);
}