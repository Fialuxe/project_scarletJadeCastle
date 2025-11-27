#include "camera.h"
#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Camera state
float camX = 0.0f;
float camY = 5.0f;
float camZ = 10.0f;
float camYaw = -90.0f; // Facing negative Z
float camPitch = 0.0f;

void applyCamera() {
  // Calculate look-at point
  float radYaw = camYaw * M_PI / 180.0f;
  float radPitch = camPitch * M_PI / 180.0f;

  float lookX = camX + cos(radPitch) * cos(radYaw);
  float lookY = camY + sin(radPitch);
  float lookZ = camZ + cos(radPitch) * sin(radYaw);

  gluLookAt(camX, camY, camZ, lookX, lookY, lookZ, 0.0f, 1.0f, 0.0f);
}

void handleKeyboard(unsigned char key, int x, int y) {
  float speed = 2.0f;
  float radYaw = camYaw * M_PI / 180.0f;

  // Forward vector (ignoring Y for movement)
  float dx = cos(radYaw);
  float dz = sin(radYaw);

  // Right vector
  float rx = -dz;
  float rz = dx;

  switch (key) {
  case 'w': // Forward
  case 'W':
    camX += dx * speed;
    camZ += dz * speed;
    break;
  case 's': // Backward
  case 'S':
    camX -= dx * speed;
    camZ -= dz * speed;
    break;
  case 'a': // Left
  case 'A':
    camX -= rx * speed;
    camZ -= rz * speed;
    break;
  case 'd': // Right
  case 'D':
    camX += rx * speed;
    camZ += rz * speed;
    break;
  case 'q': // Up
  case 'Q':
    camY += speed;
    break;
  case 'e': // Down
  case 'E':
    camY -= speed;
    break;
  case 27: // ESC
    exit(0);
    break;
  }
}

void handleSpecialKeys(int key, int x, int y) {
  float turnSpeed = 5.0f;

  switch (key) {
  case GLUT_KEY_LEFT:
    camYaw -= turnSpeed;
    break;
  case GLUT_KEY_RIGHT:
    camYaw += turnSpeed;
    break;
  case GLUT_KEY_UP:
    camPitch += turnSpeed;
    if (camPitch > 89.0f)
      camPitch = 89.0f;
    break;
  case GLUT_KEY_DOWN:
    camPitch -= turnSpeed;
    if (camPitch < -89.0f)
      camPitch = -89.0f;
    break;
  }
}
