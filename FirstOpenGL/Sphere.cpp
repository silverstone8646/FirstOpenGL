#include "Sphere.h"
#include <math.h>
#include <vector>
#include <iostream>

#define PI 3.14159265358

Sphere::Sphere() :
	m_Radius(0.5), m_PitchStep(30), m_YawStep(30)
{
}

void Sphere::SetRadius(float radius)
{
	m_Radius = radius;
}

void Sphere::SetPitchStep(int pitchStep)
{
	m_PitchStep = pitchStep;
}

void Sphere::SetYawStep(int yawStep)
{
	m_YawStep = yawStep;
}

int Sphere::GetTriangleCount() const
{
	return m_YawStep * m_PitchStep * 2;
}

int Sphere::GetVertexCount() const
{
	//为方便计算，先不将南北极看作一个点
	return (m_YawStep + 1) * (m_PitchStep + 1);
}

void Sphere::GetVertexArray(std::vector<GLfloat>& vecVertex) const
{
	float yawInterval = 360.0 / m_YawStep;
	float pitchInterval = 180.0 / m_PitchStep;

	float yaw = 0.0;
	float pitch = 0.0;

	float x = 0.0, y = 0.0, z = 0.0;

	vecVertex.clear();
	vecVertex.reserve(GetVertexCount() * 8);

	//每个顶点对应位置，法向量和纹理坐标8个值
	for (int i = 0; i <= m_YawStep; i++)
	{
		for (int j = 0; j <= m_PitchStep; j++)
		{
			yaw = yawInterval * i;
			pitch = pitchInterval * j - 90.0; 
			y = m_Radius * sin(pitch * PI / 180);
			x = m_Radius * cos(pitch * PI / 180) * cos(yaw * PI / 180);
			z = m_Radius * cos(pitch * PI / 180) * sin(yaw * PI / 180);

			//顶点
			vecVertex.push_back(x);
			vecVertex.push_back(y);
			vecVertex.push_back(z);
			
			//法向量
			vecVertex.push_back(x / m_Radius);
			vecVertex.push_back(y / m_Radius);
			vecVertex.push_back(z / m_Radius);

			//纹理坐标
			vecVertex.push_back((360-yaw) / 360);
			vecVertex.push_back((-pitch + 90) / 180);

			//std::cout << yaw / 360 << "," << (pitch + 90) / 180 << std::endl;
		}
	}
}

void Sphere::GetVertexIndices(std::vector<GLuint>& vecVertexIndices) const
{
	vecVertexIndices.clear();
	vecVertexIndices.reserve(GetTriangleCount() * 3);

	for (int i = 0; i < m_YawStep; i++)
	{
		for (int j = 0; j < m_PitchStep; j++)
		{
			//每一个点(左下角)对应两个三角形
			//****************************
			//*                       *  *
			//*                   *		 *
			//*               *			 *
			//*           *				 *	
			//*       *					 *
			//*   *						 *
			//****************************

			int temp = i == m_YawStep - 1 ? 0 : i+1;

			vecVertexIndices.push_back((m_PitchStep + 1) * i + j);
			vecVertexIndices.push_back((m_PitchStep + 1) * (i + 1) + j);
			vecVertexIndices.push_back((m_PitchStep + 1) * (i + 1) + j + 1);

			vecVertexIndices.push_back((m_PitchStep + 1) * i + j);
			vecVertexIndices.push_back((m_PitchStep + 1) * (i + 1) + j + 1);
			vecVertexIndices.push_back((m_PitchStep + 1) * i + j + 1);
		}
	}


}

