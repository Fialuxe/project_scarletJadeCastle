#ifndef INPUT_H
#define INPUT_H

#include "window.h"

void Input_Init(GLFWwindow *window);
void Input_GetMouseDelta(float *dx, float *dy);
int Input_GetKey(GLFWwindow *window, int key);

#endif
