#include <vanche_utils.h>
#include "renderer/window.h"

#include "loader/vanche_model.hpp"
#include "loader/vanche_loader.hpp"

#include "renderer/vmodelRenderer.hpp"

#include "face_pose_detection/face_recognizer.hpp"
#include "face_pose_detection/face_mapper.hpp"

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
// https://hub.vroid.com/en/characters/5355100161680925073/models/2684838785958206602
#define furisode "/home/chevan/Downloads/furisode.vrm"

static char modelPath[256];
static int camera_opened = 1;

#include <chevan_utils_array.h>

VModel_t vmodel;
float angle = 0;

void init()
{
  vmodel.path = modelPath;
  vmodel.model = loadModel(modelPath);
  WORLDInit();
  initVModel(&vmodel);
  WORLD.camera.pos.y = 1.4;
  WORLD.camera.pos.z = 0.4;
  camera_opened = recognizer_init();
  if (camera_opened && camera_opened != VANCHE_FRECOG_NO_CAMERA)
    fprintf(stderr,"Error while opening the camera. Error number %d",camera_opened);
  camera_opened = !camera_opened;
  if (camera_opened)
  {
    chferror(mapper_init);
    int err = recognizer_calibrate();
    if (err && err != VANCHE_FRECOG_NO_FACE)
      chprinterr("An error occured while calibrating the camera with code: %d", err);
  }
  else
    fprintf(stderr, "Camera failed to open. Running without face recognition\n");
}

void update()
{
  clock_t start_time, end_time;
  double total_time;
  ((void)total_time);
  start_time = clock();
  // background color
  glClearColor(0.3f, 0.3f, 0.3f, 1.f);
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
  if (camera_opened)
  {
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
      int camCode = recognizer_calibrate();
      if (camCode == VANCHE_FRECOG_CAMERA_READ_ERR)
      {
        fprintf(stderr, "Camera read error");
        exit(camCode);
      }
    }
    // get facial landmarks and movement
    int recog_err = recognizer_update();
    if (recog_err != VANCHE_FRECOG_NO_FACE)
    {
      // map facial landmarks to expressions
      chferror(map_points, facial_landmarks);

      // pass expressions to model
      vmodelSetVRMExpressions(&vmodel, model_expressions.data);

      // rotate model
      // int headNode=vmodelGetVRMNode(&vmodel,"head");
      // assert(headNode!=-1);
      // memcpy(vmodel.model.nodes[headNode].matrix, &facial_movement, sizeof(glm::mat4));
    }
  }
  end_time = clock();
  total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  // printf("FPS: %f\n",1/total_time);
}

int main(int argn,char**args)
{
  if(argn>1){
    strcpy(modelPath,args[1]);
  }else{
    strcpy(modelPath,seedsan);
  }
  printf("Vanche Start\n");
  printf("Launching with model %s\n",modelPath);
  launch();


  freeVModel(&vmodel);
  recognizer_close();

  // ch_writeFile("../dlib68Trackers.bin", (void *)tmp_face.aa, 5*68 * sizeof(glm::vec2));

  return 0;
}