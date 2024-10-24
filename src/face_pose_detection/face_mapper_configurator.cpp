#include "face_recognizer.hpp"
#ifdef WIN32
// #include <windows.h>
#else
#include <chevan_utils_macro.h>
#include <unistd.h>
#endif


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
#ifdef _WIN32
    if (!strcmp(message,"happy"))
      e=Commands::happy;
    else if(!strcmp(message,"angry"))
      e=Commands::angry;
    else if(!strcmp(message,"sad"))
      e=Commands::sad;
    else if(!strcmp(message,"relaxed"))
      e=Commands::relaxed;
    else if(!strcmp(message,"surprised"))
      e=Commands::surprised;
    else if(!strcmp(message,"aa"))
      e=Commands::aa;
    else if(!strcmp(message,"ih"))
      e=Commands::ih;
    else if(!strcmp(message,"ou"))
      e=Commands::ou;
    else if(!strcmp(message,"ee"))
      e=Commands::ee;
    else if(!strcmp(message,"oh"))
      e=Commands::oh;
    else if(!strcmp(message,"blink"))
      e=Commands::blink;
    else if(!strcmp(message,"blinkLeft"))
      e=Commands::blinkLeft;
    else if(!strcmp(message,"blinkRight"))
      e=Commands::blinkRight;
    else if(!strcmp(message,"lookUp"))
      e=Commands::lookUp;
    else if(!strcmp(message,"lookDown"))
      e=Commands::lookDown;
    else if(!strcmp(message,"lookLeft"))
      e=Commands::lookLeft;
    else if(!strcmp(message,"lookRight"))
      e=Commands::lookRight;
    else if(!strcmp(message,"neutral"))
      e=Commands::neutral;
    else if(!strcmp(message,"custom"))
      e=Commands::custom;
    else if(!strcmp(message,"cal"))
      e=Commands::cal;
    else
      e=Commands::END;
#else
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
#endif

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
#ifdef WIN32
        // Sleep(0.1);
#else
        usleep(100);
#endif
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