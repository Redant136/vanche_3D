#include <vanche_utils.h>
#include "renderer/window.h"
#include "loader/model.hpp"
#include "loader/loader.hpp"
#include "renderer/vmodelRenderer.hpp"
#include <iostream>

#define modelcamera "/home/chevan/Projects/vanche_3D/models/AntiqueCamera.glb"
#define cube "/home/chevan/Projects/vanche_3D/models/AnimatedMorphCube.glb"
#define boombox "/home/chevan/Projects/vanche_3D/models/BoomBox.glb"
#define helmet "/home/chevan/Projects/vanche_3D/models/DamagedHelmet.glb"
#define male1 "/home/chevan/Projects/vanche_3D/models/male1.glb"
#define alicia "/home/chevan/Projects/vanche_3D/models/AliciaSolid_vrm-0.51.vrm"
#define figure "/home/chevan/Projects/vanche_3D/models/RiggedFigure.gltf"
#define skinTest "/home/chevan/Projects/vanche_3D/models/SimpleSkin.gltf"
#define wave "/home/chevan/Projects/vanche_3D/models/wave/scene.gltf"
#define waveWindows "C:\\Users\\antoi\\Documents\\vanche_3D\\models\\wave\\scene.gltf"
#define modelPath "../models/AliciaSolid_vrm-0.51.vrm"   

VModel_t vmodel;
void init()
{
  vmodel.path = modelPath;
  vmodel.model = loadModel(modelPath);
  WORLDExecute(vmodel.model);
  initVModel(&vmodel);
}

void update()
{
  glClearColor(0.3f, 0.3f, 0.3f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  renderVModel(vmodel);


  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    WORLD.camera.pos.x-=0.1;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    WORLD.camera.pos.x+=0.1;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    WORLD.camera.pos.z += 0.1;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    WORLD.camera.pos.z -= 0.1;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    WORLD.camera.pos.y += 0.1;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    WORLD.camera.pos.y -= 0.1;
}
int main()
{
  printf("hello world!\n");
  launch();
  return 0;
}