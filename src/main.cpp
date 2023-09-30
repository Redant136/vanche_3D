#include <vanche_utils.h>
#include "renderer/window.h"
#include "loader/model.hpp"
#include "loader/loader.hpp"
#include "renderer/vmodelRenderer.hpp"
#include <iostream>

#define helmet "/home/chevan/Projects/vanche_3D/models/DamagedHelmet.glb"
#define male1 "/home/chevan/Projects/vanche_3D/models/male1.glb"
#define alicia "/home/chevan/Projects/vanche_3D/models/AliciaSolid_vrm-0.51.vrm"
#define figure "/home/chevan/Projects/vanche_3D/models/RiggedFigure.gltf"
#define skinTest "/home/chevan/Projects/vanche_3D/models/SimpleSkin.gltf"
#define wave "/home/chevan/Projects/vanche_3D/models/wave/scene.gltf"
#define seedsan "/home/chevan/Projects/vanche_3D/models/Seed-san.vrm"
#define female2 "/home/chevan/Projects/vanche_3D/models/VRM1_Constraint_Twist_Sample.vrm"
#define modelPath seedsan

VModel_t vmodel;
float angle = 0;
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

  float X = 0, Z = 0;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    X-=0.1;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    X+=0.1;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    Z -= 0.1;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    Z += 0.1;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    WORLD.camera.pos.y += 0.1;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    WORLD.camera.pos.y -= 0.1;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    angle += 0.1;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    angle -= 0.1;

  WORLD.camera.pos.z += X * cos(angle) + Z * sin(angle);
  WORLD.camera.pos.x += -X * sin(angle) + Z * cos(angle);

  WORLD.camera.rot.x=cos(angle/2);
  WORLD.camera.rot.y=0;
  WORLD.camera.rot.z=sin(angle/2);
  WORLD.camera.rot.w=0;
}
int main()
{
  printf("hello world!\n");
  launch();
  
  return 0;
}