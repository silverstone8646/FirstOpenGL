#ifndef DEFINES
#define DEFINES

#include <string>

const std::string ksSimpleLightVertexShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/simplelight.vert";
const std::string ksSimpleLightFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/simplelight.frag";
const std::string ksSimpleLampFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/simplelamp.frag"; //fragment shader for light
const std::string ksFloorVertexShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/floor.vert";
const std::string ksFloorFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/floor.frag";

const std::string ksSimpleDepthVertexShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/simpleDepth.vert";
const std::string ksSimpleDepthFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/simpleDepth.frag";

const std::string ksDebugDepthVertexShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/debugDepth.vert";
const std::string ksDebugDepthFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/debugDepth.frag";

const std::string ksSimpleTexVertexShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/shader.vert";
const std::string ksSimpleTexFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/shader.frag";

const std::string ksSkyboxVertexShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/cubemap.vert";
const std::string ksSkyboxFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/cubemap.frag";

const std::string ksMeshModelVertexShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/mesh.vert";
const std::string ksMeshModelFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/mesh.frag";
const std::string ksDepthPeelingFragmentShaderFile = "E:/selfstudy/OpenGL/FirstOpenGL/FirstOpenGL/depthpeeling_meshing.frag";
const std::string ksMeshModelFile = "E:/selfstudy/OpenGL/FLYFiles/nanosuit.obj";
const std::string ksDragonModelFile = "E:/selfstudy/OpenGL/MeshModels/dragon/dragon.obj";
const std::string ksChahuModelFile = "E:/selfstudy/OpenGL/MeshModels/chahu/1.obj";

//mouse state
enum MouseState
{
	Down = 0,
	Up
};

enum Mouse
{
	Left = 0,
	Middle,
	Right
};

#endif // !DEFINES



