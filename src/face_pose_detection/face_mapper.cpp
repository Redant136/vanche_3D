#include "face_mapper.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

static uint32_t TRACKER_MAP = 0x03;
#define TRACK_NEUTRAL (TRACKER_MAP & 1 << 0)
#define TRACK_MOUTH (TRACKER_MAP & 1 << 1)
#define TRACK_EYES (TRACKER_MAP & 1 << 2)
#define TRACK_PUPILS (TRACKER_MAP & 1 << 3)
#define TRACK_EMOTION (TRACKER_MAP & 1 << 4)

#ifndef VANCHE_FACEMAPPER_BINPATH
#define VANCHE_FACEMAPPER_BINPATH "../data_files/dlib68Trackers.bin"
#endif

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
union Model_Expressions_Weights_t model_expressions;
struct Model_Sensitivity_t model_sensitivity;

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
    tracker_deltas.eyes_deltas.blinkLeft = tracker_deltas.eyes_deltas.blink + 68 * 2;

    // other
    tracker_deltas.other.neutral=(glm::vec2*)calloc(2*68,sizeof(glm::vec2));
    tracker_deltas.other.custom = tracker_deltas.other.neutral + 68 * 1;

    void *tracker_deltaData=0;
    ch_bufferFile(VANCHE_FACEMAPPER_BINPATH, &tracker_deltaData, 0);
    memcpy(tracker_deltas.mouth_deltas.aa, tracker_deltaData, sizeof(glm::vec2) * 68 * 5);
  }
  return 0;
}

static int partial_deriv_solve(glm::vec2 *points)
{
#ifdef VANCHE_FACE_ENGINE_dlib68
  const float epsilon = 3;
  glm::vec2 cost[68];
  glm::vec2 estimate[68] = {glm::vec2(0, 0)};
  // get estimate
  for (int i = 0; i < 68; i++)
  {
    for (int j = 0; j < sizeof(Model_Expressions_Weights_t) / sizeof(float); j++)
    {
      if (tracker_deltas.deltas[j])
        estimate[i] += model_expressions.data[j] * tracker_deltas.deltas[j][i];
    }
  }
  // get delta
  for (int i = 0; i < 68; i++)
  {
    cost[i] = estimate[i] - points[i];
    cost[i] *= 2;
  }

  // partial deriv
  if (TRACK_NEUTRAL)
  {
    float change = 0;
    for(int i=0;i<68;i++)
      change += tracker_deltas.other.neutral[i].x * cost[i].x + tracker_deltas.other.neutral[i].y * cost[i].y;
    model_expressions.neutral = MIN(1, MAX(0, model_expressions.neutral - model_sensitivity.neutral * epsilon * change*68));
  }
  if (TRACK_MOUTH)
  {
    float changes[5] = {0};
    for (int i = 48; i < 68; i++)
    {
      changes[0] += tracker_deltas.mouth_deltas.aa[i].x * cost[i].x + tracker_deltas.mouth_deltas.aa[i].y * cost[i].y;
      changes[1] += tracker_deltas.mouth_deltas.ih[i].x * cost[i].x + tracker_deltas.mouth_deltas.ih[i].y * cost[i].y;
      changes[2] += tracker_deltas.mouth_deltas.ou[i].x * cost[i].x + tracker_deltas.mouth_deltas.ou[i].y * cost[i].y;
      changes[3] += tracker_deltas.mouth_deltas.ee[i].x * cost[i].x + tracker_deltas.mouth_deltas.ee[i].y * cost[i].y;
      changes[4] += tracker_deltas.mouth_deltas.oh[i].x * cost[i].x + tracker_deltas.mouth_deltas.oh[i].y * cost[i].y;
    }
    model_expressions.aa = MIN(1, MAX(0, model_expressions.aa - model_sensitivity.aa * epsilon * changes[0]*20));
    model_expressions.ih = MIN(1, MAX(0, model_expressions.ih - model_sensitivity.ih * epsilon * changes[1]*20));
    model_expressions.ou = MIN(1, MAX(0, model_expressions.ou - model_sensitivity.ou * epsilon * changes[2]*20));
    model_expressions.ee = MIN(1, MAX(0, model_expressions.ee - model_sensitivity.ee * epsilon * changes[3]*20));
    model_expressions.oh = MIN(1, MAX(0, model_expressions.oh - model_sensitivity.oh * epsilon * changes[4]*20));
  }
#endif
  return 0;
}

int map_points(glm::vec2 *points)
{
  if (VANCHE_FMAPPER_SOLVER == VANCHE_FMAPPER_derivSolver)
    partial_deriv_solve(points);
  return 0;
}