#version 450 core

void main()
{
	//����Ҫ��ɫ����
	gl_FragDepth = gl_FragCoord.z;
}