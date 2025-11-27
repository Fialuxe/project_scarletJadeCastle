#ifndef SHADER_H
#define SHADER_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLuint loadShader(const char *vertexPath, const char *fragmentPath);

#endif
