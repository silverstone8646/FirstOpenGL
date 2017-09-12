#include "ContainerRenderPlugin.h"
#include "LightRenderPlugin.h"
#include "CubeMapMeshRenderPlugin.h"
#include "MeshRenderPlugin.h"
#include "glut.h"

//ContainerRenderPlugin renderPlugin;
//LightRenderPlugin renderPlugin;
//CubeMapMeshRenderPlugin renderPlugin;
MeshRenderPlugin renderPlugin;

void Keyboard(unsigned char key, int x, int y)
{
	renderPlugin.Keyboard(key, x, y);
	glutPostRedisplay();
}

//left mouse: 0, middle mouse: 1, right mouse: 2
//mousedown: 0, mouse up: 1
void MouseClick(int button, int state, int x, int y)
{
	renderPlugin.MouseClick(button, state, x, y);
	
	glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
	renderPlugin.MouseMotion(x, y);

	glutPostRedisplay();
}

void myReshape(GLsizei w, GLsizei h)
{
	renderPlugin.Reshape(w, h);
	glutPostRedisplay();
}

void PreRender()
{
	renderPlugin.PreRender();
}

//render
bool first = true;
void myDisplay(void)
{
	renderPlugin.Render();

	if (!first)
	{
		glutSwapBuffers();
	}
	
	first = false;
}


void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(200, 200);

	glutCreateWindow("First OpenGL");

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		return;
	}

	//PreRender();
	renderPlugin.PreRender();

	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseClick);
	glutMotionFunc(MouseMotion);
	glutReshapeFunc(myReshape);

	glutMainLoop();
	return;
}