#include "water_fbo.h"
#include <stdio.h>
#include <stdlib.h>

// Constants for FBO sizes
// Reflection can be smaller for performance
#define REFLECTION_WIDTH 1280
#define REFLECTION_HEIGHT 720
#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 720

static GLuint createFrameBuffer() {
  GLuint frameBuffer;
  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  return frameBuffer;
}

static GLuint createTextureAttachment(int width, int height) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
  return texture;
}

static GLuint createDepthTextureAttachment(int width, int height) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
  return texture;
}

static GLuint createDepthBufferAttachment(int width, int height) {
  GLuint depthBuffer;
  glGenRenderbuffers(1, &depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthBuffer);
  return depthBuffer;
}

static void initialiseReflectionFrameBuffer(WaterFrameBuffers *fbos) {
  fbos->reflectionFrameBuffer = createFrameBuffer();
  fbos->reflectionTexture =
      createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
  fbos->reflectionDepthBuffer =
      createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("ERROR::FRAMEBUFFER:: Reflection Framebuffer is not complete!\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void initialiseRefractionFrameBuffer(WaterFrameBuffers *fbos) {
  fbos->refractionFrameBuffer = createFrameBuffer();
  fbos->refractionTexture =
      createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
  fbos->refractionDepthTexture =
      createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("ERROR::FRAMEBUFFER:: Refraction Framebuffer is not complete!\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

WaterFrameBuffers WaterFBO_Init(int width, int height) {
  WaterFrameBuffers fbos;
  initialiseReflectionFrameBuffer(&fbos);
  initialiseRefractionFrameBuffer(&fbos);
  return fbos;
}

void WaterFBO_CleanUp(WaterFrameBuffers *fbos) {
  glDeleteFramebuffers(1, &fbos->reflectionFrameBuffer);
  glDeleteTextures(1, &fbos->reflectionTexture);
  glDeleteRenderbuffers(1, &fbos->reflectionDepthBuffer);

  glDeleteFramebuffers(1, &fbos->refractionFrameBuffer);
  glDeleteTextures(1, &fbos->refractionTexture);
  glDeleteTextures(1, &fbos->refractionDepthTexture);
}

void WaterFBO_BindReflectionFrameBuffer(WaterFrameBuffers *fbos, int width,
                                        int height) {
  glBindTexture(GL_TEXTURE_2D, 0); // Make sure texture isn't bound
  glBindFramebuffer(GL_FRAMEBUFFER, fbos->reflectionFrameBuffer);
  glViewport(0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void WaterFBO_BindRefractionFrameBuffer(WaterFrameBuffers *fbos, int width,
                                        int height) {
  glBindTexture(GL_TEXTURE_2D, 0); // Make sure texture isn't bound
  glBindFramebuffer(GL_FRAMEBUFFER, fbos->refractionFrameBuffer);
  glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterFBO_UnbindCurrentFrameBuffer(int width, int height) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, width, height);
}
