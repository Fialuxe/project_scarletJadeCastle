#ifndef WATER_FBO_H
#define WATER_FBO_H

#include "../core/window.h"

typedef struct {
  GLuint reflectionFrameBuffer;
  GLuint reflectionTexture;
  GLuint reflectionDepthBuffer;

  GLuint refractionFrameBuffer;
  GLuint refractionTexture;
  GLuint refractionDepthTexture;
} WaterFrameBuffers;

WaterFrameBuffers WaterFBO_Init(int width, int height);
void WaterFBO_CleanUp(WaterFrameBuffers *fbos);
void WaterFBO_BindReflectionFrameBuffer(WaterFrameBuffers *fbos, int width,
                                        int height);
void WaterFBO_BindRefractionFrameBuffer(WaterFrameBuffers *fbos, int width,
                                        int height);
void WaterFBO_UnbindCurrentFrameBuffer(int width, int height);

#endif
