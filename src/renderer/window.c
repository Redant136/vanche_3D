#include "window.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

struct WindowData_t windowData = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 1, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
GLFWwindow *window;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glfwMakeContextCurrent(window);
  glViewport(0, 0, width, height);
}
int createWindow()
{
  if (!glfwInit())
  {
    assert(0 && "could not create context");
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "test", NULL, NULL);
  if (window == NULL)
  {
    glfwTerminate();
    assert(0 && "cannot create window");
  }
  glfwMakeContextCurrent(window);
  // glfwSetWindowPos(window, 0, 0);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    assert(0 && "could not initialize GLAD");
  }
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glEnable(GL_CULL_FACE);

  // glCullFace(GL_BACK);
  // glFrontFace(GL_CCW);

  glfwGetWindowSize(window, &windowData.realWindowWidth, &windowData.realWindowHeight);
  return 0;
}