#version 450

out vec4 color;

in VS_OUT
{
	vec3 myNormal;
	vec3 fragPos;
	vec2 TexCoords;
	vec4 fragPosInLightSpace;
}fs_in;

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

void main()
{
	vec3 normal = normalize(fs_in.myNormal);
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);

	//ambient
	vec3 ambient = material.ambient * lightColor;

	//diffuse
	float diffuseFactor = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = material.diffuse * diffuseFactor * lightColor;

	//specular
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = material.specular * specFactor * lightColor;

	vec3 result = light.ambient * ambient + (light.diffuse * diffuse + light.specular * specular);

	color = vec4(result, 1.0f);
}