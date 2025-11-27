#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "camera.h"
#include "scene.h"

// Window dimensions
int windowWidth = 1024;
int windowHeight = 768;

void init() {
  // Background color (Twilight dark blue/purple as a base)
  glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);

  setupLighting();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  applyCamera(); // Set up the camera view

  drawScene(); // Draw the map and objects

  glutSwapBuffers();
}

void reshape(int w, int h) {
  windowWidth = w;
  windowHeight = h;
  if (h == 0)
    h = 1;
  float aspect = (float)w / (float)h;

  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, aspect, 0.1, 1000.0);

  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  handleKeyboard(key, x, y);
  glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
  handleSpecialKeys(key, x, y);
  glutPostRedisplay();
}

void idle() {
  extern void updateSceneTime();
  updateSceneTime();
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(windowWidth, windowHeight);
  glutCreateWindow("Scarlet Jade Palace - Entrance");

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeys);
  glutIdleFunc(idle);

  printf("Controls:\n");
  printf("WASD: Move Camera\n");
  printf("Arrow Keys: Look Around\n");
  printf("ESC: Exit\n");

  glutMainLoop();
  return 0;
}
