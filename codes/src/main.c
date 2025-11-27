#include "config.h"
#include "core/camera.h"
#include "core/input.h"
#include "core/window.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "utils/math_utils.h"
#include <stdio.h>

int main() {
  // 1. Init Window
  GLFWwindow *window = initWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE);
  if (!window)
    return -1;

  // 2. Init Input
  Input_Init(window);

  // 3. Init Camera
  Camera camera;
  vec3 startPos = {0.0f, 1.5f, 3.0f};
  vec3 up = {0.0f, 1.0f, 0.0f};
  Camera_Init(&camera, startPos, up, -90.0f, 0.0f);

  // 4. Load Resources
  GLuint shader = Shader_Create("shaders/floor.vert", "shaders/floor.frag");
  GLuint normalMap = Texture_CreateProceduralNormalMap(512, 512);

  // Create Cube Mesh for pathways
  // Original size 100x100. Split into two 100x40 sections with 20 gap.
  // Height 2.0f.
  Mesh floorMesh = Mesh_CreateCube(100.0f, 2.0f, 40.0f);

  // 5. Lighting Config
  vec3 sunDir = {0.5f, -0.2f, 0.5f};
  vec3 sunColor = {1.0f, 0.8f, 0.7f};
  vec3 ambientColor = {0.25f, 0.1f, 0.1f};

  Shader_Use(shader);
  Shader_SetInt(shader, "normalMap", 0);
  Shader_SetVec3(shader, "sunDir", sunDir.x, sunDir.y, sunDir.z);
  Shader_SetVec3(shader, "sunColor", sunColor.x, sunColor.y, sunColor.z);
  Shader_SetVec3(shader, "ambientColor", ambientColor.x, ambientColor.y,
                 ambientColor.z);

  // 6. Main Loop
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    // Time
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Input
    if (Input_GetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, 1);

    float dx, dy;
    Input_GetMouseDelta(&dx, &dy);
    Camera_ProcessMouseMovement(&camera, dx, dy);

    if (Input_GetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_FORWARD, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_BACKWARD, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_LEFT, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_RIGHT, deltaTime);

    // Lock Camera Height
    camera.Position.y = 0.5f;

    // Render
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.7f, 0.25f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader_Use(shader);

    mat4 view = Camera_GetViewMatrix(&camera);
    mat4 proj = perspective(1.047f, (float)width / (float)height, 0.1f, 200.0f);

    Shader_SetMat4(shader, "view", view.m);
    Shader_SetMat4(shader, "projection", proj.m);
    Shader_SetVec3(shader, "viewPos", camera.Position.x, camera.Position.y,
                   camera.Position.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    // Draw Pathway 1 (Near) - Z = 22.5
    mat4 model1 = identity();
    model1.m[13] = -1.0f; // Y = -1 (Top at 0)
    model1.m[14] = 22.5f; // Z = 22.5
    Shader_SetMat4(shader, "model", model1.m);
    Mesh_Draw(&floorMesh);

    // Draw Pathway 2 (Far) - Z = -22.5
    mat4 model2 = identity();
    model2.m[13] = -1.0f;  // Y = -1
    model2.m[14] = -22.5f; // Z = -22.5
    Shader_SetMat4(shader, "model", model2.m);
    Mesh_Draw(&floorMesh);

    glfwSwapBuffers(window);
    glfwPollEvents();

    // DEBUG: Player Position (Easy to remove)
    // printf("Player Pos: %.2f, %.2f, %.2f\n", camera.Position.x,
    // camera.Position.y, camera.Position.z);
    static int frameCount = 0;
    if (frameCount++ % 60 == 0) { // Print once every 60 frames to avoid spam
      printf("Player Pos: %.2f, %.2f, %.2f\n", camera.Position.x,
             camera.Position.y, camera.Position.z);
    }
  }

  glfwTerminate();
  return 0;
}
