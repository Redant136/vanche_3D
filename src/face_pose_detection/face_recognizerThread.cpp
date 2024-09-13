#include "face_recognizer.hpp"

#define THREADDONE_MASK 0x8000

static volatile int recognizer_status;
static pthread_t thread_id;


void* threaded_update(void*_a)
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