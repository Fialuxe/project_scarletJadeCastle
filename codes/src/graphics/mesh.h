#ifndef MESH_H
#define MESH_H

#include "../core/window.h"

typedef struct {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  int indexCount;
  GLuint instanceVBO;
} Mesh;

Mesh Mesh_CreatePlane(float size);
Mesh Mesh_CreateCube(float width, float height, float depth);
Mesh Mesh_CreateCylinder(float radius, float height, int segments);
Mesh Mesh_LoadModel(const char *path);
void Mesh_Draw(Mesh *mesh);
void Mesh_SetupInstanced(Mesh *mesh, int instanceCount, const float *matrices);
void Mesh_DrawInstanced(Mesh *mesh, int instanceCount);

#endif
