#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>

Mesh Mesh_CreatePlane(float size) {
  Mesh mesh;
  float halfSize = size / 2.0f;

  // 4 vertices (Quad)
  // Pos(3), Normal(3), UV(2), Tangent(3)
  // Rotating UVs by 45 degrees to align grooves with movement if they appear
  // diagonal
  float s = size; // Texture scale
  float uv00_u = (0.0f - 0.0f) * 0.7071f;
  float uv00_v = (0.0f + 0.0f) * 0.7071f;
  float uv10_u = (s - 0.0f) * 0.7071f;
  float uv10_v = (s + 0.0f) * 0.7071f;
  float uv11_u = (s - s) * 0.7071f;
  float uv11_v = (s + s) * 0.7071f;
  float uv01_u = (0.0f - s) * 0.7071f;
  float uv01_v = (0.0f + s) * 0.7071f;

  // Rotated Tangent (45 deg around Y) -> (0.707, 0, 0.707)
  float tx = 0.7071f;
  float ty = 0.0f;
  float tz = 0.7071f;

  float vertices[] = {
      // Pos              // Normal       // UV (Rotated)       // Tangent
      // (Rotated)
      -halfSize, 0.0f, -halfSize, 0.0f, 1.0f, 0.0f, uv01_u, uv01_v, tx, ty, tz,
      halfSize,  0.0f, -halfSize, 0.0f, 1.0f, 0.0f, uv11_u, uv11_v, tx, ty, tz,
      halfSize,  0.0f, halfSize,  0.0f, 1.0f, 0.0f, uv10_u, uv10_v, tx, ty, tz,
      -halfSize, 0.0f, halfSize,  0.0f, 1.0f, 0.0f, uv00_u, uv00_v, tx, ty, tz};
  unsigned int indices[] = {0, 2, 1, 2, 3, 0}; // CCW

  mesh.indexCount = 6;

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Attribs
  int stride = 11 * sizeof(float);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)(8 * sizeof(float)));

  glBindVertexArray(0);
  return mesh;
}

Mesh Mesh_CreateCube(float width, float height, float depth) {
  Mesh mesh;
  float hw = width / 2.0f;
  float hh = height / 2.0f;
  float hd = depth / 2.0f;

  // 24 vertices (4 per face * 6 faces)
  // Pos(3), Normal(3), UV(2), Tangent(3)
  // UV scaling: 1.0 per unit? Or match plane? Plane was size=100, UV=100.
  // So 1.0 per unit.
  float uW = width;
  float uH = height;
  float uD = depth;

  // UV Rotation for Top Face (align grooves)
  float cosA = 0.7071f;
  float sinA = 0.7071f;
  // Corner 1: (0, uD)
  float uv1_u = (0.0f * cosA - uD * sinA);
  float uv1_v = (0.0f * sinA + uD * cosA);
  // Corner 2: (uW, uD)
  float uv2_u = (uW * cosA - uD * sinA);
  float uv2_v = (uW * sinA + uD * cosA);
  // Corner 3: (uW, 0)
  float uv3_u = (uW * cosA - 0.0f * sinA);
  float uv3_v = (uW * sinA + 0.0f * cosA);
  // Corner 4: (0, 0)
  float uv4_u = (0.0f * cosA - 0.0f * sinA);
  float uv4_v = (0.0f * sinA + 0.0f * cosA);

  // Rotated Tangent (45 deg around Y)
  float tx = 0.7071f;
  float ty = 0.0f;
  float tz = 0.7071f;

  // Bottom Face (y = -hh)
  // Corner 1: (0, uD)
  float uvB1_u = (0.0f * cosA - uD * sinA);
  float uvB1_v = (0.0f * sinA + uD * cosA);
  // Corner 2: (uW, uD)
  float uvB2_u = (uW * cosA - uD * sinA);
  float uvB2_v = (uW * sinA + uD * cosA);
  // Corner 3: (uW, 0)
  float uvB3_u = (uW * cosA - 0.0f * sinA);
  float uvB3_v = (uW * sinA + 0.0f * cosA);
  // Corner 4: (0, 0)
  float uvB4_u = (0.0f * cosA - 0.0f * sinA);
  float uvB4_v = (0.0f * sinA + 0.0f * cosA);

  // Front Face (z = hd) -> Width uW, Height uH
  // Corner 1: (0, uH)
  float uvF1_u = (0.0f * cosA - uH * sinA);
  float uvF1_v = (0.0f * sinA + uH * cosA);
  // Corner 2: (uW, uH)
  float uvF2_u = (uW * cosA - uH * sinA);
  float uvF2_v = (uW * sinA + uH * cosA);
  // Corner 3: (uW, 0)
  float uvF3_u = (uW * cosA - 0.0f * sinA);
  float uvF3_v = (uW * sinA + 0.0f * cosA);
  // Corner 4: (0, 0)
  float uvF4_u = (0.0f * cosA - 0.0f * sinA);
  float uvF4_v = (0.0f * sinA + 0.0f * cosA);

  // Back Face (z = -hd) -> Width uW, Height uH
  // Same as Front
  float uvK1_u = uvF1_u;
  float uvK1_v = uvF1_v;
  float uvK2_u = uvF2_u;
  float uvK2_v = uvF2_v;
  float uvK3_u = uvF3_u;
  float uvK3_v = uvF3_v;
  float uvK4_u = uvF4_u;
  float uvK4_v = uvF4_v;

  // Left Face (x = -hw) -> Width uD, Height uH
  // Corner 1: (0, uH)
  float uvL1_u = (0.0f * cosA - uH * sinA);
  float uvL1_v = (0.0f * sinA + uH * cosA);
  // Corner 2: (uD, uH)
  float uvL2_u = (uD * cosA - uH * sinA);
  float uvL2_v = (uD * sinA + uH * cosA);
  // Corner 3: (uD, 0)
  float uvL3_u = (uD * cosA - 0.0f * sinA);
  float uvL3_v = (uD * sinA + 0.0f * cosA);
  // Corner 4: (0, 0)
  float uvL4_u = (0.0f * cosA - 0.0f * sinA);
  float uvL4_v = (0.0f * sinA + 0.0f * cosA);

  // Right Face (x = hw) -> Width uD, Height uH
  // Same as Left
  float uvR1_u = uvL1_u;
  float uvR1_v = uvL1_v;
  float uvR2_u = uvL2_u;
  float uvR2_v = uvL2_v;
  float uvR3_u = uvL3_u;
  float uvR3_v = uvL3_v;
  float uvR4_u = uvL4_u;
  float uvR4_v = uvL4_v;

  float vertices[] = {
      // Top Face (y = hh)
      -hw, hh, -hd, 0.0f, 1.0f, 0.0f, uv1_u, uv1_v, tx, ty, tz, hw, hh, -hd,
      0.0f, 1.0f, 0.0f, uv2_u, uv2_v, tx, ty, tz, hw, hh, hd, 0.0f, 1.0f, 0.0f,
      uv3_u, uv3_v, tx, ty, tz, -hw, hh, hd, 0.0f, 1.0f, 0.0f, uv4_u, uv4_v, tx,
      ty, tz,

      // Bottom Face (y = -hh)
      -hw, -hh, -hd, 0.0f, -1.0f, 0.0f, uvB4_u, uvB4_v, tx, ty, tz, hw, -hh,
      -hd, 0.0f, -1.0f, 0.0f, uvB3_u, uvB3_v, tx, ty, tz, hw, -hh, hd, 0.0f,
      -1.0f, 0.0f, uvB2_u, uvB2_v, tx, ty, tz, -hw, -hh, hd, 0.0f, -1.0f, 0.0f,
      uvB1_u, uvB1_v, tx, ty, tz,

      // Front Face (z = hd)
      -hw, -hh, hd, 0.0f, 0.0f, 1.0f, uvF4_u, uvF4_v, tx, ty, tz, hw, -hh, hd,
      0.0f, 0.0f, 1.0f, uvF3_u, uvF3_v, tx, ty, tz, hw, hh, hd, 0.0f, 0.0f,
      1.0f, uvF2_u, uvF2_v, tx, ty, tz, -hw, hh, hd, 0.0f, 0.0f, 1.0f, uvF1_u,
      uvF1_v, tx, ty, tz,

      // Back Face (z = -hd)
      -hw, -hh, -hd, 0.0f, 0.0f, -1.0f, uvK4_u, uvK4_v, tx, ty, tz, hw, -hh,
      -hd, 0.0f, 0.0f, -1.0f, uvK3_u, uvK3_v, tx, ty, tz, hw, hh, -hd, 0.0f,
      0.0f, -1.0f, uvK2_u, uvK2_v, tx, ty, tz, -hw, hh, -hd, 0.0f, 0.0f, -1.0f,
      uvK1_u, uvK1_v, tx, ty, tz,

      // Left Face (x = -hw)
      -hw, -hh, -hd, -1.0f, 0.0f, 0.0f, uvL4_u, uvL4_v, tx, ty, tz, -hw, -hh,
      hd, -1.0f, 0.0f, 0.0f, uvL3_u, uvL3_v, tx, ty, tz, -hw, hh, hd, -1.0f,
      0.0f, 0.0f, uvL2_u, uvL2_v, tx, ty, tz, -hw, hh, -hd, -1.0f, 0.0f, 0.0f,
      uvL1_u, uvL1_v, tx, ty, tz,

      // Right Face (x = hw)
      hw, -hh, -hd, 1.0f, 0.0f, 0.0f, uvR4_u, uvR4_v, tx, ty, tz, hw, -hh, hd,
      1.0f, 0.0f, 0.0f, uvR3_u, uvR3_v, tx, ty, tz, hw, hh, hd, 1.0f, 0.0f,
      0.0f, uvR2_u, uvR2_v, tx, ty, tz, hw, hh, -hd, 1.0f, 0.0f, 0.0f, uvR1_u,
      uvR1_v, tx, ty, tz};

  // Add OpenGL's specification below
  //  CounterClockwise
  //  Expressed by triangle strip
  unsigned int indices[] = {
      0,  2,  1,  0,  3,  2,  // Top
      4,  5,  6,  4,  6,  7,  // Bottom
      8,  9,  10, 10, 11, 8,  // Front
      12, 14, 13, 14, 12, 15, // Back
      16, 17, 18, 18, 19, 16, // Left
      20, 22, 21, 22, 20, 23  // Right
  };

  mesh.indexCount = 36;

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  int stride = 11 * sizeof(float);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)(8 * sizeof(float)));

  glBindVertexArray(0);
  return mesh;
}

void Mesh_Draw(Mesh *mesh) {
  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stdio.h>

Mesh Mesh_LoadModel(const char *path) {
  Mesh mesh = {0};

  const struct aiScene *scene = aiImportFile(
      path, aiProcess_Triangulate | aiProcess_FlipUVs |
                aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
                aiProcess_JoinIdenticalVertices);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
    return mesh;
  }

  // Combine all meshes into one
  int numVertices = 0;
  int numIndices = 0;

  for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    numVertices += scene->mMeshes[i]->mNumVertices;
    for (unsigned int f = 0; f < scene->mMeshes[i]->mNumFaces; f++) {
      numIndices += scene->mMeshes[i]->mFaces[f].mNumIndices;
    }
  }

  printf("Mesh_LoadModel: Loading %s. Combined %d meshes. Verts: %d, Indices: "
         "%d\n",
         path, scene->mNumMeshes, numVertices, numIndices);

  // Vertex format: Pos(3), Normal(3), UV(2), Tangent(3) -> 11 floats
  int stride = 11;
  float *vertices = (float *)malloc(numVertices * stride * sizeof(float));
  unsigned int *indices =
      (unsigned int *)malloc(numIndices * sizeof(unsigned int));

  int currentVertexOffset = 0;
  int currentIndexOffset = 0;

  for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
    struct aiMesh *aMesh = scene->mMeshes[m];

    for (unsigned int i = 0; i < aMesh->mNumVertices; i++) {
      int vIdx = currentVertexOffset + i;
      // Position
      vertices[vIdx * stride + 0] = aMesh->mVertices[i].x;
      vertices[vIdx * stride + 1] = aMesh->mVertices[i].y;
      vertices[vIdx * stride + 2] = aMesh->mVertices[i].z;

      // Normal
      if (aMesh->mNormals) {
        vertices[vIdx * stride + 3] = aMesh->mNormals[i].x;
        vertices[vIdx * stride + 4] = aMesh->mNormals[i].y;
        vertices[vIdx * stride + 5] = aMesh->mNormals[i].z;
      } else {
        vertices[vIdx * stride + 3] = 0.0f;
        vertices[vIdx * stride + 4] = 1.0f;
        vertices[vIdx * stride + 5] = 0.0f;
      }

      // UV
      if (aMesh->mTextureCoords[0]) {
        vertices[vIdx * stride + 6] = aMesh->mTextureCoords[0][i].x;
        vertices[vIdx * stride + 7] = aMesh->mTextureCoords[0][i].y;
      } else {
        vertices[vIdx * stride + 6] = 0.0f;
        vertices[vIdx * stride + 7] = 0.0f;
      }

      // Tangent
      if (aMesh->mTangents) {
        vertices[vIdx * stride + 8] = aMesh->mTangents[i].x;
        vertices[vIdx * stride + 9] = aMesh->mTangents[i].y;
        vertices[vIdx * stride + 10] = aMesh->mTangents[i].z;
      } else {
        vertices[vIdx * stride + 8] = 1.0f;
        vertices[vIdx * stride + 9] = 0.0f;
        vertices[vIdx * stride + 10] = 0.0f;
      }
    }

    // Indices
    for (unsigned int f = 0; f < aMesh->mNumFaces; f++) {
      struct aiFace face = aMesh->mFaces[f];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
        indices[currentIndexOffset++] = face.mIndices[j] + currentVertexOffset;
      }
    }
    currentVertexOffset += aMesh->mNumVertices;
  }

  mesh.indexCount = numIndices;

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, numVertices * stride * sizeof(float), vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int),
               indices, GL_STATIC_DRAW);

  // Attribs
  int strideBytes = stride * sizeof(float);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideBytes, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes,
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideBytes,
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, strideBytes,
                        (void *)(8 * sizeof(float)));

  glBindVertexArray(0);

  free(vertices);
  free(indices);
  aiReleaseImport(scene);

  return mesh;
}

void Mesh_SetupInstanced(Mesh *mesh, int instanceCount, const float *matrices) {
  glGenBuffers(1, &mesh->instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(float) * 16, matrices,
               GL_STATIC_DRAW);

  glBindVertexArray(mesh->VAO);
  // Vertex Attributes
  // vec4 * 4
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        (void *)(4 * sizeof(float)));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        (void *)(8 * sizeof(float)));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        (void *)(12 * sizeof(float)));

  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);

  glBindVertexArray(0);
}

void Mesh_DrawInstanced(Mesh *mesh, int instanceCount) {
  glBindVertexArray(mesh->VAO);
  glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0,
                          instanceCount);
  glBindVertexArray(0);
}
