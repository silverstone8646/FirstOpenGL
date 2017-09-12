#ifndef SPHERE_H
#define SPHERE_H

#include "glew.h"
#include <vector>

class Sphere
{
public:
	Sphere();

	void SetRadius(float radius);
	void SetPitchStep(int pitchStep);
	void SetYawStep(int yawStep);

	int GetTriangleCount() const;
	int GetVertexCount() const;

	void GetVertexArray(std::vector<GLfloat>&) const;
	void GetVertexIndices(std::vector<GLuint>&) const;

private:
	float m_Radius; 
	int m_PitchStep; //俯仰角步数
	int m_YawStep; //偏航角步数
};

#endif
