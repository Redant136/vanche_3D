#include <vanche_utils.h>
#include "renderer/window.h"
#include "loader/model.hpp"
#include "loader/loader.hpp"
#include "renderer/vmodelRenderer.hpp"
#include <iostream>
#include <time.h>

#define helmet "../models/DamagedHelmet.glb"
#define male1 "../models/male1.glb"
#define alicia "../models/AliciaSolid_vrm-0.51.vrm"
#define figure "../models/RiggedFigure.gltf"
#define skinTest "../models/SimpleSkin.gltf"
#define wave "../models/wave/scene.gltf"
#define seedsan "../models/Seed-san.vrm"
#define female2 "../models/VRM1_Constraint_Twist_Sample.vrm"
#define modelPath seedsan

float last_time = 0;

VModel_t vmodel;
float angle = 0;
float expressionsWeights[20] = {0};
void init()
{
  vmodel.path = modelPath;
  vmodel.model = loadModel(modelPath);
  WORLDExecute(vmodel.model);
  initVModel(&vmodel);
  expressionsWeights[2] = 1;
  vmodelSetVRMExpressions(&vmodel, expressionsWeights);
}

void update()
{
  clock_t start_t, end_t;
  double total_t;
  start_t = clock();
  updateVModel(&vmodel);
  renderVModel(vmodel);

  float X = 0, Z = 0;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    X -= 0.1;
    WORLD.camera.updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    X += 0.1;
    WORLD.camera.updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    Z -= 0.1;
    WORLD.camera.updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    Z += 0.1;
    WORLD.camera.updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
  {
    WORLD.camera.pos.y += 0.1;
    WORLD.camera.updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
  {
    WORLD.camera.pos.y -= 0.1;
    WORLD.camera.updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  {
    angle += 0.1;
    WORLD.camera.updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  {
    angle -= 0.1;
    WORLD.camera.updated = true;
  }

  WORLD.camera.pos.z += X * cos(angle) + Z * sin(angle);
  WORLD.camera.pos.x += -X * sin(angle) + Z * cos(angle);
  WORLD.camera.rot.x = cos(angle / 2);
  WORLD.camera.rot.y = 0;
  WORLD.camera.rot.z = sin(angle / 2);
  WORLD.camera.rot.w = 0;
  end_t = clock();
  total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  // printf("FPS: %f\n",1/total_t);
}

int main()
{
  printf("hello world!\n");
  launch();
  freeVModel(&vmodel);
  return 0;
}