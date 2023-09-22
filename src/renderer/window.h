#ifndef VANCHE_WINDOW
#define VANCHE_WINDOW
#include <vanche_utils.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C"{
#endif


struct WindowData_t
{
  float lastX;
  float lastY;
  int firstMouse;
  float deltaTime;
  float lastFrame;
  int realWindowWidth, realWindowHeight;
};
extern struct WindowData_t windowData;
extern GLFWwindow *window;

int createWindow();
void init();
void update();
static int launch()
{
  createWindow();
  init();

  while (!glfwWindowShouldClose(window))
  {
    glfwMakeContextCurrent(window);
    glClearColor(0.3f, 0.3f, 0.3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    update();
    glfwSwapInterval(1); // v-sync
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}

#ifdef __cplusplus
}
#endif
#endif
