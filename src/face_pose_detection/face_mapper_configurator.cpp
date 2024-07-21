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

  uint numPoints = 1;
#if VANCHE_FACE_ENGINE_dlib68
  numPoints = 68;
#endif

  glm::vec2 *filebuffer = (glm::vec2 *)calloc(numPoints * END, sizeof(glm::vec2));
  free(filebuffer);
  ch_bufferFile("../data_files/dlib68Trackers_custom.bin",(void**)&filebuffer,0);
  filebuffer = (glm::vec2 *)realloc(filebuffer, numPoints * END * sizeof(glm::vec2));
  memcpy(filebuffer+5*68,filebuffer,sizeof(glm::vec2)*5*68);
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
      std::vector<glm::vec2> facial_points = std::vector<glm::vec2>();
      uint numFrames = 0;
      while ((clock() - start_time) / CLOCKS_PER_SEC < 5)
      {
        recognizer_update();
        ++numFrames;
        for (int i = 0; i < numPoints; i++)
        {
          facial_points.push_back(facial_landmarks[i]);
        }
        usleep(100);
      }
      for (int i = 0; i < numPoints; i++)
      {
        glm::vec2 sum = glm::vec2(0, 0);
        for (int j = 0; j < numFrames; j++)
        {
          sum += facial_points[i * numPoints + j];
        }
        sum /= numFrames;
        filebuffer[e*68+i] = sum;
      }
    }
  } while (e != END);

  ch_writeFile("../data_files/dlib68Trackers_custom.bin", filebuffer, sizeof(glm::vec2) * numPoints * END);
  return 0;
}