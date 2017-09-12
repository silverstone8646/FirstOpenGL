#ifndef PROGRAM_H
#define PROGRAM_H

#include "glew.h"
#include <string>

class Program
{
public:
	Program(const std::string& sVertexShader, const std::string& sFragShader);
	void Use();

	GLuint program;
};


#endif // PROGRAM_H

