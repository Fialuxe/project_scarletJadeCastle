#include "texture.h"
#include <math.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  free(texData);
  return textureID;
}

// create normal map data of noise (asphalt)
void generateNoiseNormalMapData(unsigned char *data, int width, int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      t_vec3 normal = {0.0f, 0.0f, 1.0f};

      // Add noise to normal (more frequent/grainy for asphalt)
      normal.x += ((rand() % 100) / 200.0f - 0.25f); // Increased noise range
      normal.y += ((rand() % 100) / 200.0f - 0.25f);

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

GLuint Texture_CreateNoiseNormalMap(int width, int height) {
  unsigned char *texData = (unsigned char *)malloc(width * height * 3);
  generateNoiseNormalMapData(texData, width, height);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, texData);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  free(texData);
  return textureID;
}

// create grass texture data (procedural noise)
void generateGrassTextureData(unsigned char *data, int width, int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = (y * width + x) * 3;

      // Base green color
      float r = 0.1f;
      float g = 0.5f;
      float b = 0.1f;

      // Add noise
      float noise = (rand() % 100) / 100.0f; // 0.0 to 1.0

      // Vary green intensity
      g += (noise - 0.5f) * 0.4f; // +/- 0.2 variation

      // Add some brown/yellow for realism
      r += (noise - 0.5f) * 0.1f;
      b += (noise - 0.5f) * 0.05f;

      // Clamp
      if (r < 0.0f)
        r = 0.0f;
      if (r > 1.0f)
        r = 1.0f;
      if (g < 0.0f)
        g = 0.0f;
      if (g > 1.0f)
        g = 1.0f;
      if (b < 0.0f)
        b = 0.0f;
      if (b > 1.0f)
        b = 1.0f;

      data[idx] = (unsigned char)(r * 255);
      data[idx + 1] = (unsigned char)(g * 255);
      data[idx + 2] = (unsigned char)(b * 255);
    }
  }
}

GLuint Texture_CreateGrassTexture(int width, int height) {
  unsigned char *texData = (unsigned char *)malloc(width * height * 3);
  generateGrassTextureData(texData, width, height);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, texData);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Set texture filtering to linear for smoother look
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  free(texData);
  return textureID;
}

GLuint Texture_Load(const char *path) {
  int width, height, nrChannels;
  // Flip vertically because OpenGL expects 0.0 at bottom
  // User reported texture is wrong, likely double flipped or not needed for
  // this model
  stbi_set_flip_vertically_on_load(0);
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;
    else
      format = GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
  } else {
    printf("Texture failed to load at path: %s\n", path);
    return 0;
  }
}
