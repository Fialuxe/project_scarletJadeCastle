#include "config.h"
#include "core/camera.h"
#include "core/input.h"
#include "core/window.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "utils/math_utils.h"
#include <stdio.h>
#include <stdlib.h>

// Helper to store matrix
void storeMatrix(float *buffer, int index, mat4 m) {
  for (int i = 0; i < 16; i++)
    buffer[index * 16 + i] = m.m[i];
}

/*
 *
 * This file is part of Garden of Scarlet Jade Castle project.
 * If you are interested in the project, please talk to me...
 * (you do not need to read whole code... it is too hard to read as I wrote
 * adhoc)
 *
 *
 * MIT License
 *
 * Copyright (c) 2025 Takuma Masuda
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * This file handle the main logic of the program.
 * It is the entry point of the program.
 * It is responsible for initializing the window, input, and camera.
 * It is also responsible for loading the resources and rendering the scene.
 * It is also responsible for handling the main loop and the input.
 * It is also responsible for cleaning up the resources.
 */

// --- Configuration ---
// Please do not believe it will be work ...
// Since this is personal project, I will fix minor problem by adhoc.
// For example, I fixed the optimization of loading model by adhoc.
// (Ideal: load flower model for the load Actual: Due to the machine resource,
// load several and scaling)

// Mesh Dimensions
#define FLOOR_WIDTH 4.0f
#define FLOOR_HEIGHT 2.0f
#define FLOOR_DEPTH 40.0f

#define ROAD_WIDTH 2.0f
#define ROAD_HEIGHT 2.0f
#define ROAD_DEPTH 40.0f

#define GRASS_WIDTH 30.0f
#define GRASS_HEIGHT 2.0f
#define GRASS_DEPTH 40.0f

#define BORDER_WIDTH 0.15f
#define BORDER_HEIGHT 0.15f
#define BORDER_DEPTH 40.0f

// Positions & Offsets
#define ROAD_OFFSET_X 3.0f
#define GRASS_OFFSET_X                                                         \
  19.0f // Reverted to 19.0f (Grass starts at 4.0f, adjacent to road)
#define SECTION_OFFSET_Z 22.5f

#define BORDER_Y 0.025f
#define BORDER_X_INNER 1.925f
#define BORDER_X_OUTER 4.075f

#define FLOWER_OFFSET_X_START 5.0f // Start inside grass
#define FLOWER_SPACING_X 1.5f
#define FLOWER_ROWS 1
#define FLOWER_SPACING_Z 7.0f
#define FLOWER_Y_OFFSET 0.0f // Set if model is on above or below ground

#define GAZEBO_OFFSET_X 10.0f
#define GAZEBO_OFFSET_Z 5.0f
#define GAZEBO_Y_OFFSET 0.0f

// OFFSET for bridge
#define BRIDGE_OFFSET_X 0.0f
#define BRIDGE_OFFSET_Z -0.04f
#define BRIDGE_Y_OFFSET 0.0f

// Scales
#define GAZEBO_SCALE 3.0f
#define BRIDGE_SCALE 5.0f
#define FLOWER_SCALE 2.0f

// Lighting
#define SUN_DIR_X 0.5f
#define SUN_DIR_Y -0.2f
#define SUN_DIR_Z 0.5f

#define SUN_COLOR_R 1.0f
#define SUN_COLOR_G 0.8f
#define SUN_COLOR_B 0.7f

#define AMBIENT_COLOR_R 0.25f
#define AMBIENT_COLOR_G 0.1f
#define AMBIENT_COLOR_B 0.1f

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
  GLuint instancedShader =
      Shader_Create("shaders/instanced.vert", "shaders/floor.frag");
  GLuint normalMap = Texture_CreateProceduralNormalMap(512, 512);
  GLuint asphaltNormalMap = Texture_CreateNoiseNormalMap(512, 512);
  GLuint grassTexture = Texture_CreateGrassTexture(512, 512);

  // Create Cube Mesh for pathways
  Mesh floorMesh = Mesh_CreateCube(FLOOR_WIDTH, FLOOR_HEIGHT, FLOOR_DEPTH);

  // Create Road Mesh (Asphalt)
  Mesh roadMesh = Mesh_CreateCube(ROAD_WIDTH, ROAD_HEIGHT, ROAD_DEPTH);

  // Create Grass Mesh
  Mesh grassMesh = Mesh_CreateCube(GRASS_WIDTH, GRASS_HEIGHT, GRASS_DEPTH);

  // Load Gazebo Model
  Mesh gazeboMesh = Mesh_LoadModel("../materials/gazebo/base_basic_pbr.fbx");
  // Load Gazebo Texture
  GLuint gazeboTexture =
      Texture_Load("../materials/gazebo/texture_diffuse.png");

  // Create Border Mesh (Curb)
  Mesh borderMesh = Mesh_CreateCube(BORDER_WIDTH, BORDER_HEIGHT, BORDER_DEPTH);

  // Load Bridge Model
  Mesh bridgeMesh = Mesh_LoadModel("../materials/bridge/bridge_pbr.fbx");
  // Load Bridge Texture
  GLuint bridgeTexture =
      Texture_Load("../materials/bridge/texture_diffuse.png");

  // Load Flower Model
  Mesh flowerMesh = Mesh_LoadModel("../materials/flower/flower_pbr.fbx");
  // Load Flower Texture
  GLuint flowerTexture =
      Texture_Load("../materials/flower/texture_diffuse.png");

  // --- Setup Flower Instances ---
  float flowerStartZ = 6.0f;
  float flowerEndZ = 42.5f;
  int zCount = 0;
  for (float z = flowerStartZ; z <= flowerEndZ; z += FLOWER_SPACING_Z) {
    zCount++;
  }
  int totalFlowers = FLOWER_ROWS * zCount * 4; // 4 sections (L/R, Near/Far)
  float *flowerMatrices = (float *)malloc(totalFlowers * 16 * sizeof(float));
  int fIdx = 0;

  // Left Side (Near)
  for (int row = 0; row < FLOWER_ROWS; row++) {
    float xOffset = FLOWER_OFFSET_X_START + row * FLOWER_SPACING_X;
    for (float z = flowerStartZ; z <= flowerEndZ; z += FLOWER_SPACING_Z) {
      mat4 model = identity();
      model = mat4_multiply(translate(-xOffset, FLOWER_Y_OFFSET, z), model);
      model = mat4_multiply(
          scale(FLOWER_SCALE, 1.5 * FLOWER_SCALE, 2 * FLOWER_SCALE), model);
      model = mat4_multiply(rotate_y(90.0f), model);
      model = mat4_multiply(rotate_x(90.0f), model);
      // move the model slightly below

      storeMatrix(flowerMatrices, fIdx++, model);
    }
  }
  // Right Side (Near)
  for (int row = 0; row < FLOWER_ROWS; row++) {
    float xOffset = FLOWER_OFFSET_X_START + row * FLOWER_SPACING_X;
    for (float z = flowerStartZ; z <= flowerEndZ; z += FLOWER_SPACING_Z) {
      mat4 model = identity();
      model = mat4_multiply(translate(xOffset, FLOWER_Y_OFFSET, z), model);
      model = mat4_multiply(
          scale(FLOWER_SCALE, 1.5 * FLOWER_SCALE, 2 * FLOWER_SCALE), model);
      model = mat4_multiply(rotate_y(90.0f), model);
      model = mat4_multiply(rotate_x(90.0f), model);
      storeMatrix(flowerMatrices, fIdx++, model);
    }
  }
  // Left Side (Far)
  for (int row = 0; row < FLOWER_ROWS; row++) {
    float xOffset = FLOWER_OFFSET_X_START + row * FLOWER_SPACING_X;
    for (float z = flowerStartZ; z <= flowerEndZ; z += FLOWER_SPACING_Z) {
      mat4 model = identity();
      model = mat4_multiply(translate(-xOffset, FLOWER_Y_OFFSET, -z), model);
      model = mat4_multiply(
          scale(FLOWER_SCALE, 1.5 * FLOWER_SCALE, 2 * FLOWER_SCALE), model);
      model = mat4_multiply(rotate_y(90.0f), model);
      model = mat4_multiply(rotate_x(90.0f), model);
      storeMatrix(flowerMatrices, fIdx++, model);
    }
  }
  // Right Side (Far)
  for (int row = 0; row < FLOWER_ROWS; row++) {
    float xOffset = FLOWER_OFFSET_X_START + row * FLOWER_SPACING_X;
    for (float z = flowerStartZ; z <= flowerEndZ; z += FLOWER_SPACING_Z) {
      mat4 model = identity();
      model = mat4_multiply(translate(xOffset, FLOWER_Y_OFFSET, -z), model);
      // scale z axis by 2 to make it more beautiful
      model = mat4_multiply(
          scale(FLOWER_SCALE, 1.5 * FLOWER_SCALE, 2 * FLOWER_SCALE), model);
      model = mat4_multiply(rotate_y(90.0f), model);
      model = mat4_multiply(rotate_x(90.0f), model);
      storeMatrix(flowerMatrices, fIdx++, model);
    }
  }

  Mesh_SetupInstanced(&flowerMesh, totalFlowers, flowerMatrices);
  free(flowerMatrices);

  // 5. Lighting Config
  // 5. Lighting Config
  vec3 sunDir = {SUN_DIR_X, SUN_DIR_Y, SUN_DIR_Z};
  vec3 sunColor = {SUN_COLOR_R, SUN_COLOR_G, SUN_COLOR_B};
  vec3 ambientColor = {AMBIENT_COLOR_R, AMBIENT_COLOR_G, AMBIENT_COLOR_B};

  Shader_Use(shader);
  Shader_SetInt(shader, "normalMap", 0);
  Shader_SetInt(shader, "diffuseMap", 1); // Texture unit 1 for diffuse
  Shader_SetVec3(shader, "sunDir", sunDir.x, sunDir.y, sunDir.z);
  Shader_SetVec3(shader, "sunColor", sunColor.x, sunColor.y, sunColor.z);
  Shader_SetVec3(shader, "ambientColor", ambientColor.x, ambientColor.y,
                 ambientColor.z);

  // 6. Main Loop
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  // while the window is open
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

    // Store current Y position to prevent WASD from affecting vertical position
    float savedY = camera.Position.y;

    // WASD movement (horizontal only)
    if (Input_GetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_FORWARD, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_BACKWARD, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_LEFT, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_RIGHT, deltaTime);

    // Restore Y position after WASD movement (lock to XZ plane)
    camera.Position.y = savedY;

    // Space/Shift for vertical movement only
    if (Input_GetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      camera.Position.y += 2.5f * deltaTime; // Move Up
    if (Input_GetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      camera.Position.y -= 2.5f * deltaTime; // Move Down

    // Render
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.7f, 0.25f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader_Use(shader);

    mat4 view = Camera_GetViewMatrix(&camera);
    mat4 proj = perspective(1.57f, (float)width / (float)height, 0.1f, 200.0f);

    Shader_SetMat4(shader, "view", view.m);
    Shader_SetMat4(shader, "projection", proj.m);
    Shader_SetVec3(shader, "viewPos", camera.Position.x, camera.Position.y,
                   camera.Position.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    Shader_SetInt(shader, "useNormalMap",
                  1); // Enable normal map for floor/road

    // Draw Pathway 1 (Near) - Z = 22.5
    mat4 model1 = identity();
    model1.m[13] = -1.0f; // Y = -1 (Top at 0)
    model1.m[14] = SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", model1.m);
    Shader_SetVec3(shader, "objectColor", 0.4f, 0.4f, 0.45f); // Stone Grey
    Shader_SetFloat(shader, "shininess", 32.0f);
    Shader_SetFloat(shader, "specularIntensity", 0.2f);
    Shader_SetInt(shader, "useDiffuseMap", 0); // No diffuse map
    Mesh_Draw(&floorMesh);

    // Draw Pathway 2 (Far) - Z = -22.5
    mat4 model2 = identity();
    model2.m[13] = -1.0f; // Y = -1
    model2.m[14] = -SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", model2.m);
    Shader_SetVec3(shader, "objectColor", 0.4f, 0.4f, 0.45f); // Stone Grey
    // Shininess and Specular Intensity already set for floor
    Mesh_Draw(&floorMesh);

    // --- Draw Asphalt Roads ---
    // Darker color for asphalt
    Shader_SetVec3(shader, "objectColor", 0.2f, 0.2f, 0.22f);
    // Rougher surface for asphalt
    Shader_SetFloat(shader, "shininess", 10.0f);
    Shader_SetFloat(shader, "specularIntensity", 0.1f);

    // Bind Asphalt Normal Map
    glBindTexture(GL_TEXTURE_2D, asphaltNormalMap);

    // Left Road (Near)
    mat4 modelRoad1 = identity();
    modelRoad1.m[12] = -ROAD_OFFSET_X;
    modelRoad1.m[13] = -1.0f;
    modelRoad1.m[14] = SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelRoad1.m);
    Mesh_Draw(&roadMesh);

    // Right Road (Near)
    mat4 modelRoad2 = identity();
    modelRoad2.m[12] = ROAD_OFFSET_X;
    modelRoad2.m[13] = -1.0f;
    modelRoad2.m[14] = SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelRoad2.m);
    Mesh_Draw(&roadMesh);

    // Left Road (Far)
    mat4 modelRoad3 = identity();
    modelRoad3.m[12] = -ROAD_OFFSET_X;
    modelRoad3.m[13] = -1.0f;
    modelRoad3.m[14] = -SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelRoad3.m);
    Mesh_Draw(&roadMesh);

    // Right Road (Far)
    mat4 modelRoad4 = identity();
    modelRoad4.m[12] = ROAD_OFFSET_X;
    modelRoad4.m[13] = -1.0f;
    modelRoad4.m[14] = -SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelRoad4.m);
    Mesh_Draw(&roadMesh);

    // --- Draw Road Borders (Curbs) ---
    // Material: Stone Grey
    Shader_SetVec3(shader, "objectColor", 0.7f, 0.7f, 0.7f);
    Shader_SetFloat(shader, "shininess", 32.0f);
    Shader_SetFloat(shader, "specularIntensity", 0.5f);
    Shader_SetInt(shader, "useDiffuseMap", 0); // Ensure no texture

    // Bind default normal map (flat)
    glBindTexture(GL_TEXTURE_2D, normalMap);

    float borderY = BORDER_Y;
    float borderZ_Near = SECTION_OFFSET_Z;
    float borderZ_Far = -SECTION_OFFSET_Z;

    // X positions for borders
    float borderX[] = {-BORDER_X_INNER, -BORDER_X_OUTER, BORDER_X_INNER,
                       BORDER_X_OUTER};

    // Draw Near Borders
    for (int i = 0; i < 4; i++) {
      mat4 modelBorder = identity();
      modelBorder.m[12] = borderX[i];
      modelBorder.m[13] = borderY;
      modelBorder.m[14] = borderZ_Near;
      Shader_SetMat4(shader, "model", modelBorder.m);
      Mesh_Draw(&borderMesh);
    }

    // Draw Far Borders
    for (int i = 0; i < 4; i++) {
      mat4 modelBorder = identity();
      modelBorder.m[12] = borderX[i];
      modelBorder.m[13] = borderY;
      modelBorder.m[14] = borderZ_Far;
      Shader_SetMat4(shader, "model", modelBorder.m);
      Mesh_Draw(&borderMesh);
    }

    // --- Draw Bridge ---
    // Use diffuse map
    Shader_SetInt(shader, "useDiffuseMap", 1);
    // Disable normal map for bridge (it doesn't have one loaded yet, and using
    // road's would be wrong)
    Shader_SetInt(shader, "useNormalMap", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bridgeTexture);

    mat4 modelBridge = identity();
    // Place at origin for now, or maybe slightly raised?
    // Assuming bridge connects roads, maybe at Z=0?
    // Let's place it at (0, 0, 0) and see.
    // Scale might be needed if it's too big/small.
    // magic number 0.75 for the proper scaling
    modelBridge = mat4_multiply(
        scale(0.72 * BRIDGE_SCALE, 0.75 * BRIDGE_SCALE, 0.8 * BRIDGE_SCALE),
        modelBridge);
    modelBridge = mat4_multiply(rotate_y(90.0f), modelBridge);
    modelBridge = mat4_multiply(rotate_x(90.0f), modelBridge);
    modelBridge = mat4_multiply(
        translate(BRIDGE_OFFSET_X, BRIDGE_Y_OFFSET, BRIDGE_OFFSET_Z),
        modelBridge);
    Shader_SetMat4(shader, "model", modelBridge.m);

    // Material properties for bridge
    Shader_SetVec3(shader, "objectColor", 1.0f, 1.0f,
                   1.0f); // White to show texture
    Shader_SetFloat(shader, "shininess",
                    10.0f); // Lower shininess for matte look
    Shader_SetFloat(shader, "specularIntensity",
                    0.1f); // Lower specular intensity

    Mesh_Draw(&bridgeMesh);
    for (int i = 0; i < 4; i++) {
      mat4 modelBorder = identity();
      modelBorder.m[12] = borderX[i];
      modelBorder.m[13] = borderY;
      modelBorder.m[14] = borderZ_Far;
      Shader_SetMat4(shader, "model", modelBorder.m);
      Mesh_Draw(&borderMesh);
    }

    // --- Draw Grass Fields ---
    // Use diffuse map
    Shader_SetInt(shader, "useDiffuseMap", 1);
    Shader_SetInt(shader, "useNormalMap",
                  0); // Disable normal map for grass/gazebos
    Shader_SetFloat(shader, "shininess", 5.0f);          // Low shininess
    Shader_SetFloat(shader, "specularIntensity", 0.05f); // Low specular

    // Bind Grass Texture to Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    // Use Asphalt Normal Map (Noise) for Grass too - Unit 0 is already bound to
    // asphaltNormalMap from road drawing? Wait, road drawing bound
    // asphaltNormalMap to GL_TEXTURE0. So we don't need to rebind if we want to
    // use the same normal map. But let's be explicit to be safe, or just rely
    // on state. Road drawing did: glBindTexture(GL_TEXTURE_2D,
    // asphaltNormalMap); on active texture 0. So it is bound.

    // Left Grass (Near)
    mat4 modelGrass1 = identity();
    modelGrass1.m[12] = -GRASS_OFFSET_X;
    modelGrass1.m[13] = -1.0f;
    modelGrass1.m[14] = SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelGrass1.m);
    Mesh_Draw(&grassMesh);

    // Right Grass (Near)
    mat4 modelGrass2 = identity();
    modelGrass2.m[12] = GRASS_OFFSET_X;
    modelGrass2.m[13] = -1.0f;
    modelGrass2.m[14] = SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelGrass2.m);
    Mesh_Draw(&grassMesh);

    // Left Grass (Far)
    mat4 modelGrass3 = identity();
    modelGrass3.m[12] = -GRASS_OFFSET_X;
    modelGrass3.m[13] = -1.0f;
    modelGrass3.m[14] = -SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelGrass3.m);
    Mesh_Draw(&grassMesh);

    // Right Grass (Far)
    mat4 modelGrass4 = identity();
    modelGrass4.m[12] = GRASS_OFFSET_X;
    modelGrass4.m[13] = -1.0f;
    modelGrass4.m[14] = -SECTION_OFFSET_Z;
    Shader_SetMat4(shader, "model", modelGrass4.m);
    Mesh_Draw(&grassMesh);

    // --- Draw Gazebos ---
    // Use diffuse map
    Shader_SetInt(shader, "useDiffuseMap", 1);
    Shader_SetVec3(shader, "objectColor", 1.0f, 1.0f,
                   1.0f); // White to show texture
    Shader_SetFloat(shader, "shininess", 10.0f);
    Shader_SetFloat(shader, "specularIntensity", 0.1f);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gazeboTexture);

    // Bind default normal map (flat)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    // Gazebo positions relative to grass fields
    // Grass Field Dimensions: 30.0f x 40.0f
    // Half dimensions: 15.0f, 20.0f
    // We want corners, so let's use offsets like +/- 10.0f (X) and +/- 15.0f
    // (Z)

    float offsetX = GAZEBO_OFFSET_X;
    float offsetZ = GAZEBO_OFFSET_Z;

    // Relative positions for 4 corners
    float cornersX[] = {offsetX, offsetX, -offsetX, -offsetX};
    float cornersZ[] = {offsetZ, -offsetZ, offsetZ, -offsetZ};

    // Left Field (Center x = -21.0)
    for (int i = 0; i < 4; i++) {
      mat4 modelGazebo = identity();
      // Translate: Field Center + Corner Offset
      modelGazebo = mat4_multiply(translate(-GRASS_OFFSET_X + cornersX[i],
                                            GAZEBO_Y_OFFSET, cornersZ[i]),
                                  modelGazebo);

      modelGazebo = mat4_multiply(
          scale(GAZEBO_SCALE, GAZEBO_SCALE, GAZEBO_SCALE), modelGazebo);
      // Rotate (Fix orientation - laying down)
      modelGazebo = mat4_multiply(rotate_x(90.0f), modelGazebo);

      Shader_SetMat4(shader, "model", modelGazebo.m);
      Mesh_Draw(&gazeboMesh);
    }

    // Right Field (Center x = 21.0)
    for (int i = 0; i < 4; i++) {
      mat4 modelGazebo = identity();
      // Translate: Field Center + Corner Offset
      modelGazebo = mat4_multiply(
          translate(GRASS_OFFSET_X + cornersX[i], GAZEBO_Y_OFFSET, cornersZ[i]),
          modelGazebo);

      // Scale
      modelGazebo = mat4_multiply(
          scale(GAZEBO_SCALE, GAZEBO_SCALE, GAZEBO_SCALE), modelGazebo);
      // Rotate
      modelGazebo = mat4_multiply(rotate_x(90.0f), modelGazebo);

      Shader_SetMat4(shader, "model", modelGazebo.m);
      Mesh_Draw(&gazeboMesh);
    }

    // --- Draw Flowers (Instanced) ---
    Shader_Use(instancedShader);
    Shader_SetMat4(instancedShader, "view", view.m);
    Shader_SetMat4(instancedShader, "projection", proj.m);

    Shader_SetInt(instancedShader, "diffuseMap", 1); // Texture unit 1
    Shader_SetVec3(instancedShader, "sunDir", sunDir.x, sunDir.y, sunDir.z);
    Shader_SetVec3(instancedShader, "sunColor", sunColor.x, sunColor.y,
                   sunColor.z);
    Shader_SetVec3(instancedShader, "ambientColor", ambientColor.x,
                   ambientColor.y, ambientColor.z);
    Shader_SetVec3(instancedShader, "viewPos", camera.Position.x,
                   camera.Position.y, camera.Position.z);

    Shader_SetInt(instancedShader, "useDiffuseMap", 1);
    Shader_SetInt(instancedShader, "useNormalMap", 0);
    Shader_SetFloat(instancedShader, "shininess", 10.0f);
    Shader_SetFloat(instancedShader, "specularIntensity", 0.1f);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, flowerTexture);

    Mesh_DrawInstanced(&flowerMesh, totalFlowers);

    // Reset Active Texture to 0
    glActiveTexture(GL_TEXTURE0);

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
