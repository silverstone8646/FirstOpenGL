#version 450

in vec2 TexCoords;
in vec3 myNormal;
in vec3 fragPos;
out vec4 color;

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 lightPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform samplerCube skybox;

//uniform int renderMode;

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
	vec3 normal = normalize(myNormal);
	vec3 lightDir = normalize(lightPos - fragPos); //���߷���
	vec3 reflectDir = reflect(-lightDir, normal); //���߷��䷽��
	vec3 viewDir = normalize(viewPos - fragPos); //���߷���
	vec3 viewReflectDir = reflect(-viewDir, normal); //���߷��䷽��
	float refractRatio = 1.0 / 1.5;
	vec3 viewRefractDir = refract(-viewDir, normal, refractRatio); //�������䷽��

		//��������
		//ambient
		//vec3 ambient = lightColor;

		//diffuse
		//float diffuseFactor = max(dot(normal, lightDir), 0.0f);
		//vec3 diffuse = diffuseFactor * vec3(texture(texture_diffuse1, TexCoords)) * lightColor;

		//specular
		//float specFactor = pow(max(dot(viewDir, reflectDir), 0.0f), 12);
		//vec3 specular = specFactor * vec3(texture(texture_specular1, TexCoords)) * lightColor;

		//vec3 result = (light.ambient * ambient + light.diffuse * diffuse + light.specular * specular);

		//color = vec4(result, 1.0f);

		//�������
		color = texture(skybox, viewReflectDir);

		//�������
		//color = texture(skybox, viewRefractDir);

}