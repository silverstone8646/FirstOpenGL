#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glew.h"

class Shader
{
public:
	Shader(const GLchar* filePath);
	std::string& GetShaderCode();

private:
	std::string m_shaderCode;
};

#endif // ! SHADER_H

