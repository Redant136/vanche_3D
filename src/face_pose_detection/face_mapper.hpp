#ifndef VANCHE_FACE_MAPPER
#define VANCHE_FACE_MAPPER

#include <vanche_utils.h>
#include "../renderer/vmodelRenderer.hpp"

enum VANCHE_faceEngine
{
  VANCHE_Engine_dlib68 = 68
};

#if VANCHE_FACE_ENGINE_dlib68
const static enum VANCHE_faceEngine FACEENGINE = VANCHE_Engine_dlib68;
#else
#define VANCHE_FACE_ENGINE_dlib68 1
const static enum VANCHE_faceEngine FACEENGINE = VANCHE_Engine_dlib68;
#endif

enum VANCHE_FACEMAPPER_SOLVER_t
{
  VANCHE_FMAPPER_derivSolver
};
#if defined(VANCHE_FACEMAPPER_DERIVSOLVER)
const static enum VANCHE_FACEMAPPER_SOLVER_t VANCHE_FMAPPER_SOLVER = VANCHE_FMAPPER_derivSolver;
#else
#define VANCHE_FACEMAPPER_DERIVSOLVER ;
const static enum VANCHE_FACEMAPPER_SOLVER_t VANCHE_FMAPPER_SOLVER = VANCHE_FMAPPER_derivSolver;
#endif

extern union Model_Expressions_Weights_t
{
  struct
  {
    float happy, angry, sad, relaxed, surprised, aa, ih, ou, ee, oh, blink, blinkLeft, blinkRight, lookUp, lookDown, lookLeft, lookRight, neutral, custom;
  };
  struct
  {
    struct
    {
      float happy, angry, sad, relaxed, surprised;
    } emotions;
    struct
    {
      float aa, ih, ou, ee, oh;
    } vowels;
    struct
    {
      float blink, blinkLeft, blinkRight;
    } eyeBlink;
    struct
    {
      float lookUp, lookDown, lookLeft, lookRight;
    } look;
    struct
    {
      float neutral, custom;
    } other;
  };
  float data[1];
} model_expressions;
extern struct Model_Sensitivity_t{
    float happy=1, angry=1, sad=1, relaxed=1, surprised=1, aa=1, ih=1, ou=1, ee=1, oh=1, 
      blink=1, blinkLeft=1, blinkRight=1, lookUp=1, lookDown=1, lookLeft=1, lookRight=1, neutral=1, custom=1;
} model_sensitivity;

#define VANCHE_FMAPPER_UNSUPPORTED_TRACKING 1

int mapper_init();
int map_points(glm::vec2*points);

#endif