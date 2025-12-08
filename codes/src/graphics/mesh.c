/*
partially optimized performance
*/
#include "mesh.h"
#include <math.h>
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
  unsigned int indices[] = {0, 2, 1, 0, 3, 2}; // CCW

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple OBJ Loader (Replaces Assimp)
Mesh Mesh_LoadModel(const char *path) {
  Mesh mesh = {0};
  FILE *file = fopen(path, "r");
  if (!file) {
    printf("ERROR::OBJ::Could not open file: %s\n", path);
    return mesh;
  }

  // Dynamic arrays for parsing
  int vCap = 4096, vtCap = 4096, vnCap = 4096, fCap = 4096;
  int vCount = 0, vtCount = 0, vnCount = 0, fCount = 0;

  float *temp_v = (float *)malloc(vCap * 3 * sizeof(float));
  float *temp_vt = (float *)malloc(vtCap * 2 * sizeof(float));
  float *temp_vn = (float *)malloc(vnCap * 3 * sizeof(float));

  // Face structure: 3 vertices per face (triangulated)
  typedef struct {
    int v[3], vt[3], vn[3];
  } Face;
  Face *temp_f = (Face *)malloc(fCap * sizeof(Face));

  char line[512];
  int lineCount = 0;
  while (fgets(line, sizeof(line), file)) {
    lineCount++;
    if (lineCount <= 5) {
      printf("DEBUG::OBJ::Line %d: %s", lineCount, line);
    }
    // Vertex Position
    if (strncmp(line, "v ", 2) == 0) {
      if (vCount >= vCap) {
        vCap *= 2;
        temp_v = (float *)realloc(temp_v, vCap * 3 * sizeof(float));
      }
      sscanf(line, "v %f %f %f", &temp_v[vCount * 3], &temp_v[vCount * 3 + 1],
             &temp_v[vCount * 3 + 2]);
      vCount++;
    }
    // Texture Coordinate
    else if (strncmp(line, "vt ", 3) == 0) {
      if (vtCount >= vtCap) {
        vtCap *= 2;
        temp_vt = (float *)realloc(temp_vt, vtCap * 2 * sizeof(float));
      }
      sscanf(line, "vt %f %f", &temp_vt[vtCount * 2],
             &temp_vt[vtCount * 2 + 1]);
      vtCount++;
    }
    // Vertex Normal
    else if (strncmp(line, "vn ", 3) == 0) {
      if (vnCount >= vnCap) {
        vnCap *= 2;
        temp_vn = (float *)realloc(temp_vn, vnCap * 3 * sizeof(float));
      }
      sscanf(line, "vn %f %f %f", &temp_vn[vnCount * 3],
             &temp_vn[vnCount * 3 + 1], &temp_vn[vnCount * 3 + 2]);
      vnCount++;
    }
    // Face
    else if (strncmp(line, "f ", 2) == 0) {
      // Parse face indices
      int v[4] = {0}, vt[4] = {0}, vn[4] = {0};
      int count = 0;
      char *token = strtok(line + 2, " \t\r\n");
      while (token != NULL && count < 4) {
        // Try v/vt/vn
        if (sscanf(token, "%d/%d/%d", &v[count], &vt[count], &vn[count]) != 3) {
          // Try v//vn
          if (sscanf(token, "%d//%d", &v[count], &vn[count]) != 2) {
            // Try v/vt
            if (sscanf(token, "%d/%d", &v[count], &vt[count]) != 2) {
              // Try v
              sscanf(token, "%d", &v[count]);
            }
          }
        }
        count++;
        token = strtok(NULL, " \t\r\n");
      }

      // Triangulate (Fan triangulation: 0-1-2, 0-2-3)
      for (int i = 0; i < count - 2; i++) {
        if (fCount >= fCap) {
          fCap *= 2;
          temp_f = (Face *)realloc(temp_f, fCap * sizeof(Face));
        }
        // Triangle 1: 0, i+1, i+2
        int idxs[3] = {0, i + 1, i + 2};
        for (int k = 0; k < 3; k++) {
          int idx = idxs[k];
          temp_f[fCount].v[k] = (v[idx] > 0) ? v[idx] - 1 : 0;
          temp_f[fCount].vt[k] = (vt[idx] > 0) ? vt[idx] - 1 : 0;
          temp_f[fCount].vn[k] = (vn[idx] > 0) ? vn[idx] - 1 : 0;
        }
        fCount++;
      }
    }
  }
  fclose(file);

  printf("Mesh_LoadModel: Loaded %s. Verts: %d, UVs: %d, Normals: %d, Faces: "
         "%d\n",
         path, vCount, vtCount, vnCount, fCount);

  // Construct final mesh (Triangle Soup)
  int numVertices = fCount * 3;
  int numIndices = fCount * 3;
  int stride = 11; // Pos(3), Normal(3), UV(2), Tangent(3)

  float *vertices = (float *)malloc(numVertices * stride * sizeof(float));
  unsigned int *indices =
      (unsigned int *)malloc(numIndices * sizeof(unsigned int));

  for (int i = 0; i < fCount; i++) {
    // Calculate Tangent for the triangle
    float p1[3], p2[3], p3[3];
    float uv1[2], uv2[2], uv3[2];

    // Get positions and UVs
    int vIdx1 = temp_f[i].v[0];
    int vIdx2 = temp_f[i].v[1];
    int vIdx3 = temp_f[i].v[2];

    p1[0] = temp_v[vIdx1 * 3];
    p1[1] = temp_v[vIdx1 * 3 + 1];
    p1[2] = temp_v[vIdx1 * 3 + 2];
    p2[0] = temp_v[vIdx2 * 3];
    p2[1] = temp_v[vIdx2 * 3 + 1];
    p2[2] = temp_v[vIdx2 * 3 + 2];
    p3[0] = temp_v[vIdx3 * 3];
    p3[1] = temp_v[vIdx3 * 3 + 1];
    p3[2] = temp_v[vIdx3 * 3 + 2];

    int vtIdx1 = temp_f[i].vt[0];
    int vtIdx2 = temp_f[i].vt[1];
    int vtIdx3 = temp_f[i].vt[2];

    if (vtCount > 0) {
      uv1[0] = temp_vt[vtIdx1 * 2];
      uv1[1] = 1.0f - temp_vt[vtIdx1 * 2 + 1];
      uv2[0] = temp_vt[vtIdx2 * 2];
      uv2[1] = 1.0f - temp_vt[vtIdx2 * 2 + 1];
      uv3[0] = temp_vt[vtIdx3 * 2];
      uv3[1] = 1.0f - temp_vt[vtIdx3 * 2 + 1];
    } else {
      uv1[0] = 0;
      uv1[1] = 0;
      uv2[0] = 0;
      uv2[1] = 0;
      uv3[0] = 0;
      uv3[1] = 0;
    }

    // Tangent Calculation
    float edge1[3], edge2[3];
    edge1[0] = p2[0] - p1[0];
    edge1[1] = p2[1] - p1[1];
    edge1[2] = p2[2] - p1[2];
    edge2[0] = p3[0] - p1[0];
    edge2[1] = p3[1] - p1[1];
    edge2[2] = p3[2] - p1[2];

    float deltaUV1[2], deltaUV2[2];
    deltaUV1[0] = uv2[0] - uv1[0];
    deltaUV1[1] = uv2[1] - uv1[1];
    deltaUV2[0] = uv3[0] - uv1[0];
    deltaUV2[1] = uv3[1] - uv1[1];

    float f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);
    if (isinf(f) || isnan(f))
      f = 1.0f; // Safety

    float tangent[3];
    tangent[0] = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
    tangent[1] = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
    tangent[2] = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);

    // Normalize tangent
    float len = sqrtf(tangent[0] * tangent[0] + tangent[1] * tangent[1] +
                      tangent[2] * tangent[2]);
    if (len > 0.0001f) {
      tangent[0] /= len;
      tangent[1] /= len;
      tangent[2] /= len;
    } else {
      tangent[0] = 1.0f;
      tangent[1] = 0.0f;
      tangent[2] = 0.0f;
    }

    for (int j = 0; j < 3; j++) {
      int vIdx = i * 3 + j;
      int vIndex = temp_f[i].v[j];
      int vtIndex = temp_f[i].vt[j];
      int vnIndex = temp_f[i].vn[j];

      // Pos
      vertices[vIdx * stride + 0] = temp_v[vIndex * 3];
      vertices[vIdx * stride + 1] = temp_v[vIndex * 3 + 1];
      vertices[vIdx * stride + 2] = temp_v[vIndex * 3 + 2];

      // Normal
      if (vnCount > 0) {
        vertices[vIdx * stride + 3] = temp_vn[vnIndex * 3];
        vertices[vIdx * stride + 4] = temp_vn[vnIndex * 3 + 1];
        vertices[vIdx * stride + 5] = temp_vn[vnIndex * 3 + 2];
      } else {
        vertices[vIdx * stride + 3] = 0.0f;
        vertices[vIdx * stride + 4] = 1.0f;
        vertices[vIdx * stride + 5] = 0.0f;
      }

      // UV
      if (vtCount > 0) {
        vertices[vIdx * stride + 6] = temp_vt[vtIndex * 2];
        vertices[vIdx * stride + 7] = 1.0f - temp_vt[vtIndex * 2 + 1]; // Flip V
      } else {
        vertices[vIdx * stride + 6] = 0.0f;
        vertices[vIdx * stride + 7] = 0.0f;
      }

      // Tangent
      vertices[vIdx * stride + 8] = tangent[0];
      vertices[vIdx * stride + 9] = tangent[1];
      vertices[vIdx * stride + 10] = tangent[2];

      indices[vIdx] = vIdx;
    }
  }

  // Cleanup temp arrays
  free(temp_v);
  free(temp_vt);
  free(temp_vn);
  free(temp_f);

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

Mesh Mesh_CreateCylinder(float radius, float height, int segments) {
  Mesh mesh;
  int vertexCount = (segments + 1) * 2; // Top and bottom rings
  int indexCount = segments * 6;        // 2 triangles per segment

  float *vertices = (float *)malloc(vertexCount * 11 * sizeof(float));
  unsigned int *indices =
      (unsigned int *)malloc(indexCount * sizeof(unsigned int));

  float halfHeight = height / 2.0f;

  for (int i = 0; i <= segments; i++) {
    float theta = (float)i / (float)segments * 2.0f * M_PI;
    float x = radius * cosf(theta);
    float z = radius * sinf(theta);

    // Bottom vertex
    int baseIdx = i * 2;
    // Pos
    vertices[baseIdx * 11 + 0] = x;
    vertices[baseIdx * 11 + 1] = -halfHeight;
    vertices[baseIdx * 11 + 2] = z;
    // Normal (pointing out)
    vertices[baseIdx * 11 + 3] = x / radius;
    vertices[baseIdx * 11 + 4] = 0.0f;
    vertices[baseIdx * 11 + 5] = z / radius;
    // UV
    vertices[baseIdx * 11 + 6] = (float)i / (float)segments;
    vertices[baseIdx * 11 + 7] = 0.0f;
    // Tangent (approx)
    vertices[baseIdx * 11 + 8] = -sinf(theta);
    vertices[baseIdx * 11 + 9] = 0.0f;
    vertices[baseIdx * 11 + 10] = cosf(theta);

    // Top vertex
    int topIdx = i * 2 + 1;
    // Pos
    vertices[topIdx * 11 + 0] = x;
    vertices[topIdx * 11 + 1] = halfHeight;
    vertices[topIdx * 11 + 2] = z;
    // Normal
    vertices[topIdx * 11 + 3] = x / radius;
    vertices[topIdx * 11 + 4] = 0.0f;
    vertices[topIdx * 11 + 5] = z / radius;
    // UV
    vertices[topIdx * 11 + 6] = (float)i / (float)segments;
    vertices[topIdx * 11 + 7] = 1.0f;
    // Tangent
    vertices[topIdx * 11 + 8] = -sinf(theta);
    vertices[topIdx * 11 + 9] = 0.0f;
    vertices[topIdx * 11 + 10] = cosf(theta);

    // Indices
    if (i < segments) {
      int idx = i * 6;
      indices[idx + 0] = baseIdx;
      indices[idx + 1] = baseIdx + 2; // Next bottom
      indices[idx + 2] = topIdx;

      indices[idx + 3] = topIdx;
      indices[idx + 4] = baseIdx + 2; // Next bottom
      indices[idx + 5] = topIdx + 2;  // Next top
    }
  }

  mesh.indexCount = indexCount;

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexCount * 11 * sizeof(float), vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int),
               indices, GL_STATIC_DRAW);

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
  free(vertices);
  free(indices);

  return mesh;
}
