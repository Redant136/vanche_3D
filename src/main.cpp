#include <vanche_utils.h>
#include "renderer/window.h"
#include "loader/model.hpp"
#include "loader/loader.hpp"
#include "renderer/vmodelRenderer.hpp"
#include "face_pose_detection/face_recognizer.hpp"
#include "face_pose_detection/face_mapper.hpp"
#include <iostream>
#include <time.h>
#include <glm/gtx/string_cast.hpp>

#define helmet "../models/DamagedHelmet.glb"
#define male1 "../models/male1.glb"
#define alicia "../models/AliciaSolid_vrm-0.51.vrm"
#define figure "../models/RiggedFigure.gltf"
#define skinTest "../models/SimpleSkin.gltf"
#define wave "../models/wave/scene.gltf"
#define seedsan "../models/Seed-san.vrm"
#define female2 "../models/VRM1_Constraint_Twist_Sample.vrm"
#define monkey "../models/SuzanneMorphSparse.glb"
#define antoinegameing "/home/chevan/Downloads/antoinegaming.glb"
#define modelPath seedsan

static float last_time = 0;
static int camera_opened = 1;

VModel_t vmodel;
float angle = 0;

struct
{
  int aa_n, ih_n, ou_n, ee_n, oh_n;
  glm::vec2 aa[68], ih[68], ou[68], ee[68], oh[68];
} tmp_face;

void init()
{
  vmodel.path = modelPath;
  vmodel.model = loadModel(modelPath);
  WORLDExecute(vmodel.model);
  initVModel(&vmodel);
  WORLD.camera.pos.y = 1.4;
  WORLD.camera.pos.z = 0.4;
  // id of the camera to be opened
  camera_opened = recognizer_init(0);
  if (camera_opened && camera_opened != VANCHE_FRECOG_NO_CAMERA)
    fprintf(stderr,"Error while opening camera: %d",camera_opened);

  camera_opened = !camera_opened;
  if (camera_opened)
  {
    chferror(mapper_init);
    chferror(recognizer_calibrate);
  }
  else
    fprintf(stderr, "Camera failed to open. Running without face recognition\n");

  memset(&tmp_face,0,sizeof(tmp_face));
}

void update()
{
  clock_t start_t, end_t;
  double total_t;
  start_t = clock();
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
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
  {
    int camCode = recognizer_calibrate();
    if (camCode == VANCHE_FRECOG_CAMERA_READ_ERR)
    {
      fprintf(stderr, "Camera read error");
      exit(camCode);
    }
  }

  WORLD.camera.pos.z += X * cos(angle) + Z * sin(angle);
  WORLD.camera.pos.x += -X * sin(angle) + Z * cos(angle);
  WORLD.camera.rot.x = cos(angle / 2);
  WORLD.camera.rot.y = 0;
  WORLD.camera.rot.z = sin(angle / 2);
  WORLD.camera.rot.w = 0;
  if (camera_opened)
  {
    // get facial landmarks and movement
    int recog_err = recognizer_update();
    if (recog_err != VANCHE_FRECOG_NO_FACE)
    {
      // get face transforms (delet later)
      if(1)
      {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
          tmp_face.aa_n++;
          for (int i = 0; i < 68; i++)
          {
            tmp_face.aa[i] += facial_landmarks[i];
          }
        }
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
          tmp_face.ih_n++;
          for (int i = 0; i < 68; i++)
          {
            tmp_face.ih[i] += facial_landmarks[i];
          }
        }
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        {
          tmp_face.ou_n++;
          for (int i = 0; i < 68; i++)
          {
            tmp_face.ou[i] += facial_landmarks[i];
          }
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
          tmp_face.ee_n++;
          for (int i = 0; i < 68; i++)
          {
            tmp_face.ee[i] += facial_landmarks[i];
          }
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
          tmp_face.oh_n++;
          for (int i = 0; i < 68; i++)
          {
            tmp_face.oh[i] += facial_landmarks[i];
          }
        }
      }
      
      // map facial landmarks to expressions
      chferror(map_points, facial_landmarks);

      // pass expressions to model
      vmodelSetVRMExpressions(&vmodel, model_expressions.data);
    }
  }
  end_t = clock();
  total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  // printf("FPS: %f\n",1/total_t);
}

int main()
{
  printf("Vanche Start\n");
  launch();
  freeVModel(&vmodel);
  recognizer_close();

  for(int i=0;i<68;i++){
    tmp_face.aa[i] /= tmp_face.aa_n;
    tmp_face.ih[i] /= tmp_face.ih_n;
    tmp_face.ou[i] /= tmp_face.ou_n;
    tmp_face.ee[i] /= tmp_face.ee_n;
    tmp_face.oh[i] /= tmp_face.oh_n;
  }

  // ch_writeFile("../dlib68Trackers.bin", (void *)tmp_face.aa, 5*68 * sizeof(glm::vec2));

  // std::cout<< "aa={";
  // for (int i = 0; i < 68; i++)
  // {
  //   std::cout << glm::to_string(tmp_face.aa[i])<<",";
  // }
  // std::cout << "}" << std::endl;
  // std::cout<< "ih={";
  // for (int i = 0; i < 68; i++)
  // {
  //   std::cout << glm::to_string(tmp_face.ih[i])<<",";
  // }
  // std::cout << "}" << std::endl;
  // std::cout<< "ou={";
  // for (int i = 0; i < 68; i++)
  // {
  //   std::cout << glm::to_string(tmp_face.ou[i])<<",";
  // }
  // std::cout << "}" << std::endl;
  // std::cout<< "ee={";
  // for (int i = 0; i < 68; i++)
  // {
  //   std::cout << glm::to_string(tmp_face.ee[i])<<",";
  // }
  // std::cout << "}" << std::endl;
  // std::cout<< "oh={";
  // for (int i = 0; i < 68; i++)
  // {
  //   std::cout << glm::to_string(tmp_face.oh[i])<<",";
  // }
  // std::cout << "}" << std::endl;

  return 0;
}