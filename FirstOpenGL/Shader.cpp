#include "Shader.h"

Shader::Shader(const GLchar* filePath)
{
	// 1. 从文件路径中获取顶点/片段着色器
	std::ifstream shaderFile;
	// 保证ifstream对象可以抛出异常：
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// 打开文件
		shaderFile.open(filePath);
		std::stringstream vShaderStream;
		// 读取文件的缓冲内容到流中
		vShaderStream << shaderFile.rdbuf();
		// 关闭文件
		shaderFile.close();
		// 转换流至GLchar数组
		m_shaderCode = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

}

std::string& Shader::GetShaderCode()
{
	return m_shaderCode;
}
