#ifndef VANCHE_FACE_MAPPER
#define VANCHE_FACE_MAPPER

#include <vanche_utils.h>
#include "../renderer/vmodelRenderer.hpp"

enum VANCHE_faceEngine
{
  VANCHE_Engine_dlib68 = 68
};

#if defined(VANCHE_FACE_ENGINE_dlib68)
const static enum VANCHE_faceEngine FACEENGINE = VANCHE_Engine_dlib68;
#else
#define VANCHE_FACE_ENGINE_dlib68
const static enum VANCHE_faceEngine FACEENGINE = VANCHE_Engine_dlib68;
#endif

extern union Model_Expressions_Weights_t
{
  struct
  {
    float happy, angry, sad, relaxed, surprised,
        aa, ih, ou, ee, oh,
        blink, blinkLeft, blinkRight,
        lookUp, lookDown, lookLeft, lookRight,
        neutral, custom;
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
  float data[0];
} model_expressions;

#define VANCHE_FMAPPER_UNSUPPORTED_TRACKING 1

int mapper_init();
int map_points(glm::vec2*points);

#endif