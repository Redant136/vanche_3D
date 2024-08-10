#include "face_recognizer.hpp"
#include <chevan_utils_macro.h>
#include <unistd.h>

enum Commands
{
  happy,
  angry,
  sad,
  relaxed,
  surprised,
  aa,
  ih,
  ou,
  ee,
  oh,
  blink,
  blinkLeft,
  blinkRight,
  lookUp,
  lookDown,
  lookLeft,
  lookRight,
  neutral,
  custom,
  END,
  cal
};

int main(int argn, char **args)
{
  int camera_opened = recognizer_init();
  if (camera_opened && camera_opened != VANCHE_FRECOG_NO_CAMERA)
    fprintf(stderr, "Error while opening the camera. Error number %d", camera_opened);
  camera_opened = !camera_opened;
  if (camera_opened)
  {
    int err = recognizer_calibrate();
    if (err && err != VANCHE_FRECOG_NO_FACE)
      chprinterr("An error occured while calibrating the camera with code: %d", err);
  }
  else
  {
    fprintf(stderr, "Failed to open the camera\n");
    return 1;
  }

#if VANCHE_FACE_ENGINE_dlib68
  const uint numPoints = 68;
#else
  const uint numPoints = 1;
#endif

  void *buffer = 0;
  size_t bufSize = 0;
  struct
  {
    glm::vec2 sums[numPoints * END] = {{0, 0}};
    int nPoints[END] = {0};
  } filebuffer;
  glm::vec2 cleanBuffer[numPoints * END] = {{0, 0}};
  ch_bufferFile("../data_files/dlib68Trackers_customSums.bin", (void **)&buffer, &bufSize);
  memcpy(&filebuffer, buffer, bufSize);
  Commands e = END;
  chprintln("Started successfully");
  do
  {
    chprintln("enter the expression you want to save: ");
    string128 message;
    scanf("%s", message);
    e = CH_ENUM_PARSE(message, Commands::, happy,
                      angry,
                      sad,
                      relaxed,
                      surprised,
                      aa,
                      ih,
                      ou,
                      ee,
                      oh,
                      blink,
                      blinkLeft,
                      blinkRight,
                      lookUp,
                      lookDown,
                      lookLeft,
                      lookRight,
                      neutral,
                      custom,
                      cal,
                      END);

    if (e == cal)
      recognizer_calibrate();
    else if (e < END)
    {
      clock_t start_time = clock();
      while ((clock() - start_time) / CLOCKS_PER_SEC < 5)
      {
        recognizer_update();
        filebuffer.nPoints[e]++;
        for (int i = 0; i < numPoints; i++)
        {
          filebuffer.sums[e * numPoints + i] += facial_landmarks[i];
        }
        usleep(100);
      }
      for (int i = 0; i < numPoints; i++)
      {
        cleanBuffer[e * 68 + i] = filebuffer.sums[e * 68 + i] / (float)filebuffer.nPoints[e];
      }
    }
  } while (e != END);
  ch_writeFile("../data_files/dlib68Trackers_customSums.bin", &filebuffer, sizeof(filebuffer));
  ch_writeFile("../data_files/dlib68Trackers_custom.bin", cleanBuffer, sizeof(cleanBuffer));
  return 0;
}