#include "face_recognizer.hpp"
#define THREADDONE_MASK 0x8000
#ifndef _WIN32
#include <pthread.h>
static volatile int recognizer_status;
static pthread_t thread_id;


static void* threaded_update(void*_a)
{
  recognizer_status = recognizer_update();
  recognizer_status |= THREADDONE_MASK;
  return 0;
}
void recognizer_asyncUpdate()
{
  recognizer_status = 0;
  pthread_create(&thread_id, NULL, threaded_update, NULL);
}

int recognizer_asyncJoin(int*status)
{
  if(recognizer_status & THREADDONE_MASK)
  {
    *status = recognizer_status & (~THREADDONE_MASK);
    pthread_join(thread_id, NULL);
    return 1;
  }
  return 0;
}
#else
#include <windows.h>

static volatile int recognizer_status;
static HANDLE thread;

static DWORD WINAPI threaded_update(LPVOID _a)
{
  recognizer_status = recognizer_update();
  recognizer_status |= THREADDONE_MASK;
  return 0;
}
void recognizer_asyncUpdate()
{
  recognizer_status = 0;
  thread = CreateThread(NULL, 0, threaded_update, NULL, 0, NULL);
  if (thread == NULL)
    recognizer_status = THREADDONE_MASK | VANCHE_FRECOG_CAMERA_READ_ERR;
}

int recognizer_asyncJoin(int *status)
{
  if (recognizer_status & THREADDONE_MASK)
  {
    *status = recognizer_status & (~THREADDONE_MASK);
    WaitForSingleObject(thread, INFINITE);
    return 1;
  }
  return 0;
}
#endif