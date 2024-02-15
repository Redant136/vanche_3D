#ifndef VANCHE_FACE_RECOGNIZER
#define VANCHE_FACE_RECOGNIZER
#include <vanche_utils.h>

extern glm::vec2 *facial_landmarks;
extern glm::mat4 facial_movement;

#define VANCHE_FRECOG_NO_CAMERA 1
#define VANCHE_FRECOG_CAMERA_READ_ERR 2
#define VANCHE_FRECOG_NO_FACE 3

int recognizer_init(int cameraID = -1);
int recognizer_calibrate();
int recognizer_update();
int recognizer_close();
#endif