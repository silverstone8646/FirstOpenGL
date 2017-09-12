#version 450 core

void main()
{
	//不需要颜色缓冲
	gl_FragDepth = gl_FragCoord.z;
}