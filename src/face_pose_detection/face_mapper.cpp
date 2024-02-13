#include "face_mapper.hpp"
#include <glm/gtx/string_cast.hpp>

static uint32_t TRACKER_MAP = 0b00001;
#define TRACK_MOUTH (TRACKER_MAP & 1 << 0)
#define TRACK_EYES (TRACKER_MAP & 1 << 1)
#define TRACK_PUPILS (TRACKER_MAP & 1 << 2)
#define TRACK_EMOTION (TRACKER_MAP & 1 << 3)

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
  glm::vec2 *deltas[0];
} tracker_deltas;
union Model_Expressions_Weights_t model_expressions;

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

    // time 5 cause 5 vowels
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




    void *tracker_deltaData;
    ch_bufferFile("../dlib68Trackers.bin", &tracker_deltaData, 0);
    memcpy(tracker_deltas.mouth_deltas.aa,tracker_deltaData,sizeof(glm::vec2)*68*5);
  }
  return 0;
}

static int partial_deriv_solve(glm::vec2 *points)
{

#ifdef VANCHE_FACE_ENGINE_dlib68
  const float epsilon = 100;
  glm::vec2 cost[68];
  glm::vec2 estimate[68] = {glm::vec2(0, 0)};
  // get estimate
  for (int i = 0; i < 68; i++)
  {
    for (int j = 0; j < sizeof(model_expressions) / sizeof(float); j++)
    {
      if (tracker_deltas.deltas[j])
        estimate[i] += model_expressions.data[j] * tracker_deltas.deltas[j][i];
    }
  }
  // get delta
  for (int i = 0; i < 68; i++)
  {
    cost[i] = points[i] - estimate[i];
  }
  // partial deriv

  // mouth
  if(TRACK_MOUTH){
    float changes[5]={0};
    for (int i = 48; i < 68; i++)
    {
      changes[0] += tracker_deltas.mouth_deltas.aa[i].x * cost[i].x + tracker_deltas.mouth_deltas.aa[i].y * cost[i].y;
      changes[1] += tracker_deltas.mouth_deltas.ih[i].x * cost[i].x + tracker_deltas.mouth_deltas.ih[i].y * cost[i].y;
      changes[2] += tracker_deltas.mouth_deltas.ou[i].x * cost[i].x + tracker_deltas.mouth_deltas.ou[i].y * cost[i].y;
      changes[3] += tracker_deltas.mouth_deltas.ee[i].x * cost[i].x + tracker_deltas.mouth_deltas.ee[i].y * cost[i].y;
      changes[4] += tracker_deltas.mouth_deltas.oh[i].x * cost[i].x + tracker_deltas.mouth_deltas.oh[i].y * cost[i].y;
    }
    model_expressions.aa = MAX(model_expressions.aa + epsilon * changes[0], 0);
    model_expressions.ih = MAX(model_expressions.ih + epsilon * changes[1], 0);
    model_expressions.ou = MAX(model_expressions.ou + epsilon * changes[2], 0);
    model_expressions.ee = MAX(model_expressions.ee + epsilon * changes[3], 0);
    model_expressions.oh = MAX(model_expressions.oh + epsilon * changes[4], 0);
  }
#endif
  return 0;
}

int map_points(glm::vec2 *points)
{
  partial_deriv_solve(points);
  return 0;
}

#if 0

  glm::vec2 aa[] = {
      /*48:*/ glm::vec2(-6, 0),
      /*49:*/ glm::vec2(-5, 2),
      /*50:*/ glm::vec2(-2, 5),
      /*51:*/ glm::vec2(0, 5),
      /*52:*/ glm::vec2(2, 5),
      /*53:*/ glm::vec2(5, 2),
      /*54:*/ glm::vec2(6, 0),

      /*55:*/ glm::vec2(4, 12),
      /*56:*/ glm::vec2(2, 16),
      /*57:*/ glm::vec2(0, 17),
      /*58:*/ glm::vec2(-2, 16),
      /*59:*/ glm::vec2(-4, 12),

      /*60:*/ glm::vec2(-4, 1),
      /*61:*/ glm::vec2(-2, 2),
      /*62:*/ glm::vec2(0, 2),
      /*63:*/ glm::vec2(2, 2),
      /*64:*/ glm::vec2(4, 1),
      /*65:*/ glm::vec2(3, 18),
      /*66:*/ glm::vec2(0, 19),
      /*67:*/ glm::vec2(-3, 18)};
  memcpy(mouthVowelsOffsets.aa, aa, sizeof(aa));

#endif