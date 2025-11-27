#ifndef MESH_H
#define MESH_H

#include "../core/window.h"

typedef struct {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  int indexCount;
} Mesh;

Mesh Mesh_CreatePlane(float size);
Mesh Mesh_CreateCube(float width, float height, float depth);
void Mesh_Draw(Mesh *mesh);

#endif
