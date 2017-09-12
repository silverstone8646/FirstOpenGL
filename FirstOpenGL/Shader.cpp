#include "Shader.h"

Shader::Shader(const GLchar* filePath)
{
	// 1. ���ļ�·���л�ȡ����/Ƭ����ɫ��
	std::ifstream shaderFile;
	// ��֤ifstream��������׳��쳣��
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// ���ļ�
		shaderFile.open(filePath);
		std::stringstream vShaderStream;
		// ��ȡ�ļ��Ļ������ݵ�����
		vShaderStream << shaderFile.rdbuf();
		// �ر��ļ�
		shaderFile.close();
		// ת������GLchar����
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
