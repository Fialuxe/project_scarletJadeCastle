#include "texture.h"
#include <math.h>
#include <stdlib.h>

// Helper struct for internal vec3 use
typedef struct {
  float x, y, z;
} t_vec3;

// create normal map data of tiles
// this function is used to create normal map data of tiles
// normal map is used to simulate the effect of light on the surface of the tile
// normal map is created by using the tile size of 64x64
void generateNormalMapData(unsigned char *data, int width, int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Tile size = 64
      int tx = x % 64;
      int ty = y % 64;

      t_vec3 normal = {0.0f, 0.0f, 1.0f};

      // Edge detection for grout normals
      if (tx < 4)
        normal.x = -0.5f;
      if (tx > 60)
        normal.x = 0.5f;
      if (ty < 4)
        normal.y = -0.5f;
      if (ty > 60)
        normal.y = 0.5f;

      // Add some noise to normal
      normal.x += ((rand() % 100) / 500.0f - 0.1f);
      normal.y += ((rand() % 100) / 500.0f - 0.1f);

      // Normalize
      float len = sqrtf(normal.x * normal.x + normal.y * normal.y +
                        normal.z * normal.z);
      normal.x /= len;
      normal.y /= len;
      normal.z /= len;

      // Pack to [0, 255]
      int idx = (y * width + x) * 3;
      data[idx] = (unsigned char)((normal.x * 0.5f + 0.5f) * 255);
      data[idx + 1] = (unsigned char)((normal.y * 0.5f + 0.5f) * 255);
      data[idx + 2] = (unsigned char)((normal.z * 0.5f + 0.5f) * 255);
    }
  }
}

GLuint Texture_CreateProceduralNormalMap(int width, int height) {
  unsigned char *texData = (unsigned char *)malloc(width * height * 3);
  generateNormalMapData(texData, width, height);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, texData);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  free(texData);
  return textureID;
}
