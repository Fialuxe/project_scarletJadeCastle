#ifndef TEXTURE_H
#define TEXTURE_H

#include "../core/window.h"

GLuint Texture_CreateProceduralNormalMap(int width, int height);
GLuint Texture_CreateNoiseNormalMap(int width, int height);
GLuint Texture_CreateGrassTexture(int width, int height);
GLuint Texture_Load(const char *path);

#endif
