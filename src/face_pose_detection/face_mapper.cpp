#include "face_mapper.hpp"
#include <iostream>

static uint32_t TRACKER_MAP = 0b00011;
#define TRACK_NEUTRAL (TRACKER_MAP & 1 << 0)
#define TRACK_MOUTH (TRACKER_MAP & 1 << 1)
#define TRACK_EYES (TRACKER_MAP & 1 << 2)
#define TRACK_PUPILS (TRACKER_MAP & 1 << 3)
#define TRACK_EMOTION (TRACKER_MAP & 1 << 4)

#define VANCHE_FACEMAPPER_BINPATH "../data_files/dlib68Trackers.bin"
// #define VANCHE_FACEMAPPER_BINPATH "../data_files/dlib68Trackers_custom.bin"

union
{
  struct
  {
    struct
    {
      glm::vec2 *happy;
      glm::vec2 *angry;
      glm::vec2 *sad;
      glm::vec2 *relaxed;
      glm::vec2 *surprised;
    } emotions_deltas;
    struct
    {
      glm::vec2 *aa;
      glm::vec2 *ih;
      glm::vec2 *ou;
      glm::vec2 *ee;
      glm::vec2 *oh;
    } mouth_deltas;
    struct
    {
      glm::vec2 *blink;
      glm::vec2 *blinkLeft;
      glm::vec2 *blinkRight;
    } eyes_deltas;
    struct
    {
      glm::vec2 *lookUp;
      glm::vec2 *lookDown;
      glm::vec2 *lookLeft;
      glm::vec2 *lookRight;
    } pupils_deltas;
    struct
    {
      glm::vec2 *neutral;
      glm::vec2 *custom;
    } other;
  };
  glm::vec2 *deltas[1];
} tracker_deltas;
union Model_Expressions_Weights_t intern_model_expressions;
union Model_Expressions_Weights_t model_expressions;
struct Model_Sensitivity_t model_sensitivity;
struct Model_Sensitivity_t model_scaling;

int mapper_init()
{
  if (FACEENGINE == VANCHE_Engine_dlib68)
  {
    if (TRACK_PUPILS)
    {
      fprintf(stderr, "Chosen engine does not support pupil tracking");
      return VANCHE_FMAPPER_UNSUPPORTED_TRACKING;
    }

    // emotions
    tracker_deltas.emotions_deltas.happy = (glm::vec2 *)calloc(5 * 68, sizeof(glm::vec2));
    tracker_deltas.emotions_deltas.angry = tracker_deltas.emotions_deltas.happy + 68 * 1;
    tracker_deltas.emotions_deltas.sad = tracker_deltas.emotions_deltas.happy + 68 * 2;
    tracker_deltas.emotions_deltas.relaxed = tracker_deltas.emotions_deltas.happy + 68 * 3;
    tracker_deltas.emotions_deltas.surprised = tracker_deltas.emotions_deltas.happy + 68 * 4;

    // mouth
    tracker_deltas.mouth_deltas.aa = (glm::vec2 *)calloc(5 * 68, sizeof(glm::vec2));
    tracker_deltas.mouth_deltas.ih = tracker_deltas.mouth_deltas.aa + 68 * 1;
    tracker_deltas.mouth_deltas.ou = tracker_deltas.mouth_deltas.aa + 68 * 2;
    tracker_deltas.mouth_deltas.ee = tracker_deltas.mouth_deltas.aa + 68 * 3;
    tracker_deltas.mouth_deltas.oh = tracker_deltas.mouth_deltas.aa + 68 * 4;

    // blinking
    // 36-41, 42-47
    tracker_deltas.eyes_deltas.blink = (glm::vec2 *)calloc(3 * 68, sizeof(glm::vec2));
    tracker_deltas.eyes_deltas.blinkLeft = tracker_deltas.eyes_deltas.blink + 68 * 1;
    tracker_deltas.eyes_deltas.blinkRight = tracker_deltas.eyes_deltas.blink + 68 * 2;
    if(TRACK_EYES)
    {
      fprintf(stderr,"Chosen engine does not track blinking properly, some errors might occur");
    }

    // other
    tracker_deltas.other.neutral = (glm::vec2 *)calloc(2 * 68, sizeof(glm::vec2));
    tracker_deltas.other.custom = tracker_deltas.other.neutral + 68 * 1;

    glm::vec2 *tracker_deltaData = 0;
    ch_bufferFile(VANCHE_FACEMAPPER_BINPATH, (void**)&tracker_deltaData, 0);

    memcpy(tracker_deltas.emotions_deltas.happy, tracker_deltaData, sizeof(glm::vec2) * 68 * 5);
    memcpy(tracker_deltas.mouth_deltas.aa, tracker_deltaData + 68 * 5, sizeof(glm::vec2) * 68 * 5);
    memcpy(tracker_deltas.eyes_deltas.blink, tracker_deltaData + 68 * 10, sizeof(glm::vec2) * 68 * 3);
    memcpy(tracker_deltas.other.neutral, tracker_deltaData + 68 * 17, sizeof(glm::vec2) * 68 * 2);

    free(tracker_deltaData);
  }
  return 0;
}

static int partial_deriv_solve(glm::vec2 *points)
{
  // model_sensitivity.blinkLeft=20;
#if VANCHE_FACE_ENGINE_dlib68
  const float epsilon = 3;
  for (int epoch = 0; epoch < 136; epoch++)
  {
    glm::vec2 cost[68];
    glm::vec2 estimate[68] = {glm::vec2(0, 0)};
    // get estimate
    for (int i = 0; i < 68; i++)
    {
      for (int j = 0; j < sizeof(Model_Expressions_Weights_t) / sizeof(float); j++)
      {
        if (tracker_deltas.deltas[j])
          estimate[i] += intern_model_expressions.data[j] * tracker_deltas.deltas[j][i] * 1.f;
      }
    }
    // get delta
    for (int i = 0; i < 68; i++)
    {
      cost[i] = estimate[i] - points[i];
    }

    // partial deriv
    if (TRACK_NEUTRAL && tracker_deltas.other.neutral)
    {
      float change = 0;
      for (int i = 0; i < 68; i++)
        change += tracker_deltas.other.neutral[i].x * cost[i].x + tracker_deltas.other.neutral[i].y * cost[i].y;
      intern_model_expressions.neutral -= model_sensitivity.neutral * epsilon * change;
      intern_model_expressions.neutral = MIN(1, MAX(0, intern_model_expressions.neutral));
    }
    if (TRACK_MOUTH && tracker_deltas.mouth_deltas.aa)
    {
      float changes[5] = {0};
      for (int i = 0; i < 68; i++)
      {
        if (i == 17)
          i = 48;
        changes[0] += tracker_deltas.mouth_deltas.aa[i].x * cost[i].x + tracker_deltas.mouth_deltas.aa[i].y * cost[i].y;
        changes[1] += tracker_deltas.mouth_deltas.ih[i].x * cost[i].x + tracker_deltas.mouth_deltas.ih[i].y * cost[i].y;
        changes[2] += tracker_deltas.mouth_deltas.ou[i].x * cost[i].x + tracker_deltas.mouth_deltas.ou[i].y * cost[i].y;
        changes[3] += tracker_deltas.mouth_deltas.ee[i].x * cost[i].x + tracker_deltas.mouth_deltas.ee[i].y * cost[i].y;
        changes[4] += tracker_deltas.mouth_deltas.oh[i].x * cost[i].x + tracker_deltas.mouth_deltas.oh[i].y * cost[i].y;
      }

      intern_model_expressions.aa -= model_sensitivity.aa * epsilon * changes[0];
      intern_model_expressions.ih -= model_sensitivity.ih * epsilon * changes[1];
      intern_model_expressions.ou -= model_sensitivity.ou * epsilon * changes[2];
      intern_model_expressions.ee -= model_sensitivity.ee * epsilon * changes[3];
      intern_model_expressions.oh -= model_sensitivity.oh * epsilon * changes[4];

      // chprintln("aa ", intern_model_expressions.aa);
      // chprintln("ih ", intern_model_expressions.ih);
      // chprintln("ou ", intern_model_expressions.ou);
      // chprintln("ee ", intern_model_expressions.ee);
      // chprintln("oh ", intern_model_expressions.oh);
      // chprintln();
      intern_model_expressions.aa = MIN(1, MAX(0, intern_model_expressions.aa));
      intern_model_expressions.ih = MIN(1, MAX(0, intern_model_expressions.ih));
      intern_model_expressions.ou = MIN(1, MAX(0, intern_model_expressions.ou));
      intern_model_expressions.ee = MIN(1, MAX(0, intern_model_expressions.ee));
      intern_model_expressions.oh = MIN(1, MAX(0, intern_model_expressions.oh));
    }
    if (TRACK_EYES && tracker_deltas.eyes_deltas.blink)
    {
      float changes[3] = {0};
      for (int i = 17; i < 48; i++)
      {
        if (i == 27)
          i = 36;
        changes[0] += tracker_deltas.eyes_deltas.blink[i].x * cost[i].x + tracker_deltas.eyes_deltas.blink[i].y * cost[i].y;
        changes[1] += tracker_deltas.eyes_deltas.blinkLeft[i].x * cost[i].x + tracker_deltas.eyes_deltas.blinkLeft[i].y * cost[i].y;
        changes[2] += tracker_deltas.eyes_deltas.blinkRight[i].x * cost[i].x + tracker_deltas.eyes_deltas.blinkRight[i].y * cost[i].y;
      }
      intern_model_expressions.blink -= model_sensitivity.blink * epsilon * changes[0];
      intern_model_expressions.blinkLeft -= model_sensitivity.blinkLeft * epsilon * changes[1];
      intern_model_expressions.blinkRight -= model_sensitivity.blinkRight * epsilon * changes[2];

      intern_model_expressions.blink = MIN(1, MAX(0, intern_model_expressions.blink));
      intern_model_expressions.blinkLeft = MIN(1, MAX(0, intern_model_expressions.blinkLeft));
      intern_model_expressions.blinkRight = MIN(1, MAX(0, intern_model_expressions.blinkRight));
    }
  }
#endif
  memcpy(&model_expressions, &intern_model_expressions, sizeof(Model_Expressions_Weights_t));
  for (int i = 0; i < sizeof(Model_Expressions_Weights_t) / sizeof(float); i++)
  {
    model_expressions.data[i] *= ((float *)&model_scaling)[i];
    model_expressions.data[i] = MIN(1, model_expressions.data[i]);
  }

  return 0;
}

int map_points(glm::vec2 *points)
{
  if (VANCHE_FMAPPER_SOLVER == VANCHE_FMAPPER_derivSolver)
    partial_deriv_solve(points);
  return 0;
}