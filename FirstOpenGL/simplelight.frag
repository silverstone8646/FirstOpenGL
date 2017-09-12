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

//material�е�ϵ���ǲ�ͬ���ߵ���ɫ
//Light�е�ϵ�����ǲ�ͬ������������һ����Ҫ���Ե�ϵ��
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

uniform sampler2D objTexture;
uniform sampler2D shadowMap;

float CalculateShadow(vec4 fragPosInLightSpace)
{
	//light space�µ�͸�ӳ���
	vec3 projCoordsInLight = fragPosInLightSpace.xyz / fragPosInLightSpace.w;

	//shadowMap��ȡֵΪ[0��1], Ϊ��֮�Ƚϣ�projCoordsInLight���[-1, 1]�任��[0,1]
	projCoordsInLight = projCoordsInLight * 0.5 + 0.5;

	//��ȡ��ǰƬԴ��ӦshadowMap�е����ֵ
	float recordedDepth = texture(shadowMap, projCoordsInLight.xy).r;

	//��ǰƬԴ�����ֵ
	float currentDepth = projCoordsInLight.z;

	//��֤����Ӱ����ƬԪ���ʼ��С��depthMap��Ӧλ�ü�¼�����
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	float bias = max(0.005, 0.05*(1.0 - dot(fs_in.myNormal, lightDir)));

	float shadow = 0.0;
	vec2 texSize = 1.0 / textureSize(shadowMap, 0);
	for(int i = -1; i<=1; ++i)
	{
		for(int j= -1; j<=1; ++j)
		{
			float depth = texture(shadowMap, projCoordsInLight.xy + vec2(i,j)*texSize).r;
			shadow += currentDepth-bias > depth  ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

void main()
{
	vec3 normal = normalize(fs_in.myNormal);
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);

	//������Ӱ
	float shadow = CalculateShadow(fs_in.fragPosInLightSpace); 

	vec3 textureColor = vec3(texture(objTexture, fs_in.TexCoords));

	//ambient
	vec3 ambient = material.ambient * lightColor;
	if(textureColor != vec3(0.0f))
	{
		ambient *= textureColor;
	}

	//diffuse
	float diffuseFactor = max(dot(normal, lightDir), 0.0f);
	if(textureColor == vec3(0.0f))
	{
		textureColor = vec3(1.0f);
	}
	vec3 diffuse = material.diffuse * diffuseFactor * textureColor * lightColor;
	//��Ӱ���ֵ�סdiffuse��
	diffuse = (1.0-shadow * 0.8) * diffuse;

	//specular
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = material.specular * specFactor * lightColor;
	//��Ӱ��ȫ��סspecular��
	specular = (1.0-shadow)*specular;

	vec3 result = light.ambient * ambient + (light.diffuse * diffuse + light.specular * specular);

	color = vec4(result, 1.0f);
}