#include "input.h"

static float lastX = 640.0f;
static float lastY = 360.0f;
static int firstMouse = 1;
static float deltaX = 0.0f;
static float deltaY = 0.0f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  (void)window;
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = 0;
  }

  deltaX = xpos - lastX;
  deltaY = lastY - ypos; // Reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;
}

void Input_Init(GLFWwindow *window) {
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input_GetMouseDelta(float *dx, float *dy) {
  *dx = deltaX;
  *dy = deltaY;
  // Reset delta after reading?
  // Usually processed once per frame.
  // For simplicity, we'll assume this is called once per frame loop or we reset
  // it manually. Better: The callback accumulates, and we reset here.
  deltaX = 0.0f;
  deltaY = 0.0f;
}

int Input_GetKey(GLFWwindow *window, int key) {
  return glfwGetKey(window, key);
}
