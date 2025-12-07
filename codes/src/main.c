#include "config.h"
#include "core/camera.h"
#include "core/input.h"
#include "core/window.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/water_fbo.h"
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
 * Copyright (c) 2025 Takuma Masuda
 *
 * This work is licensed under the Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License. To view a
 * copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/
 *
 * If you are interested in the project, please talk to me...
 * (you do not need to read whole code... it is too hard to read as I wrote
 * adhoc)
 *
 * tried to use PBR based shader, but it is not working well (not good quality)
 * so chosen non-PBR based shader. it may because the world is not real-world
 * based (from anime)
 * * NOTE: This project utilizes 3D models generated via Deemos Technologies
 * Inc.'s Services (ChatAvatar/Rodin), which comply with the CC BY-NC-SA 4.0
 * license selected for this project. Full attribution details are provided in
 * ATTRIBUTION.txt.
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
// All values are set by adhoc
#define FLOOR_WIDTH 4.0f
#define FLOOR_HEIGHT 2.0f
#define FLOOR_DEPTH 42.0f

#define ROAD_WIDTH 2.0f
#define ROAD_HEIGHT 2.0f
#define ROAD_DEPTH 42.0f

#define GRASS_WIDTH 30.0f
#define GRASS_HEIGHT 2.0f
#define GRASS_DEPTH 42.0f

#define BORDER_WIDTH 0.15f
#define BORDER_HEIGHT 0.15f
#define BORDER_DEPTH 42.0f

// Positions & Offsets
#define ROAD_OFFSET_X 3.0f
#define GRASS_OFFSET_X                                                         \
  19.0f // Reverted to 19.0f (Grass starts at 4.0f, adjacent to road)
#define SECTION_OFFSET_Z 22.5f

#define BORDER_Y 0.025f
#define BORDER_X_INNER 1.925f
#define BORDER_X_OUTER 4.075f

#define FLOWER_OFFSET_X_START 5.0f // Start inside grass
#define FLOWER_SPACING_X 5.0f
#define FLOWER_ROWS 2
#define FLOWER_SPACING_Z 7.0f
#define FLOWER_Y_OFFSET 0.0f // Set if model is on above or below ground
#define FLOWER_SCALE 2.0f

#define GAZEBO_OFFSET_X 10.0f
#define GAZEBO_OFFSET_Z 5.0f
#define GAZEBO_Y_OFFSET 0.0f
#define GAZEBO_SCALE 3.0f

// World Boundaries
#define WORLD_LIMIT_X 4.0f
#define WORLD_LIMIT_Z 40.0f
#define WORLD_MIN_Y 1.5f
// OFFSET for bridge
#define BRIDGE_OFFSET_X 0.0f
#define BRIDGE_OFFSET_Z -0.04f
#define BRIDGE_Y_OFFSET 0.0f
#define BRIDGE_SCALE 5.0f

#define WATER_HEIGHT -0.3f

#define GODRAY_OFFSET_Y 5.1f

// Outer Floor (Next to Grass)
// Grass ends at 4.0 + 30.0 = 34.0.
// Floor Width 4.0. Center at 34.0 + 2.0 = 36.0.
#define OUTER_FLOOR_OFFSET_X 36.0f

// Outer Grass (Next to Outer Floor)
// Outer Floor ends at 36.0 + 2.0 = 38.0.
// Grass Width 30.0. Center at 38.0 + 15.0 = 53.0.
#define OUTER_GRASS_OFFSET_X 53.0f

// Hedge moved out by Grass Size (30) + Floor Width (4) = 34.0
// Original 35.0 + 34.0 = 69.0
#define HEDGE_OFFSET_X 69.0f
#define HEDGE_SPACING_Z 16.0f
#define HEDGE_SCALE 0.05f
#define HEDGE_Y_OFFSET -1.0f

#define FENCE_WIDTH 0.2f
#define FENCE_HEIGHT 9.0f
#define FENCE_DEPTH 10.0f // Slightly longer to ensure overlap/no gap
#define FENCE_Y_OFFSET 0.0f

#define HALFPIPE_OFFSET_X 0.0f
#define HALFPIPE_OFFSET_Y                                                      \
  -0.22f // it is not intuitive... minus is above, plus is below
#define HALFPIPE_OFFSET_Z 0.0f
#define HALFPIPE_SCALE 2.65f

// Lighting
#define SUN_DIR_X -0.5f
#define SUN_DIR_Y -0.05f
#define SUN_DIR_Z -0.5f

#define SUN_COLOR_R 1.2f
#define SUN_COLOR_G 0.7f
#define SUN_COLOR_B 0.3f

#define SKY_COLOR_R 0.2f
#define SKY_COLOR_G 0.2f
#define SKY_COLOR_B 0.4f

#define GROUND_COLOR_R 0.1f
#define GROUND_COLOR_G 0.05f
#define GROUND_COLOR_B 0.05f

// Bridge Physics Constants
#define BRIDGE_Z_START -1.5f
#define BRIDGE_Z_END 1.5f
#define BRIDGE_PEAK_Y 2.5f
#define BRIDGE_WALK_WIDTH 1.5f

// --- Helper Functions ---

// Calculates the ground height at a given position (handling bridge arch)
float GetGroundHeight(float x, float z) {
  // Check if on bridge
  if (z > BRIDGE_Z_START && z < BRIDGE_Z_END) {
    // Simple parabolic arch: y = Peak - k * z^2
    // We want y = WORLD_MIN_Y at z = +/- 2.0
    // 1.5 = 2.5 - k * (2.0)^2
    // -1.0 = -k * 4.0
    // k = 0.25
    float k = 0.25f;
    float bridgeHeight = BRIDGE_PEAK_Y - k * z * z;
    return (bridgeHeight > WORLD_MIN_Y) ? bridgeHeight : WORLD_MIN_Y;
  }
  return WORLD_MIN_Y;
}

// Calculates the walkable width at a given Z position
float GetWalkableWidth(float z) {
  // Check if on bridge
  if (z > BRIDGE_Z_START && z < BRIDGE_Z_END) {
    return BRIDGE_WALK_WIDTH;
  }
  return WORLD_LIMIT_X;
}

// Draws a mesh at a specific position with no rotation/scaling (Identity basis)
void DrawMeshSimple(GLuint shader, Mesh *mesh, float x, float y, float z) {
  mat4 model = identity();
  model.m[12] = x;
  model.m[13] = y;
  model.m[14] = z;
  Shader_SetMat4(shader, "model", model.m);
  Mesh_Draw(mesh);
}

// Draws 4 symmetric instances of a mesh:
// (x, y, z), (-x, y, z), (x, y, -z), (-x, y, -z)
void DrawSymmetricLayer(GLuint shader, Mesh *mesh, float offsetX, float y,
                        float offsetZ) {
  DrawMeshSimple(shader, mesh, -offsetX, y, offsetZ);  // Left Near
  DrawMeshSimple(shader, mesh, offsetX, y, offsetZ);   // Right Near
  DrawMeshSimple(shader, mesh, -offsetX, y, -offsetZ); // Left Far
  DrawMeshSimple(shader, mesh, offsetX, y, -offsetZ);  // Right Far
  DrawMeshSimple(shader, mesh, offsetX, y, -offsetZ);  // Right Far
}

// Loads a model and its associated texture
void LoadModelWithTexture(const char *modelPath, const char *texturePath,
                          Mesh *outMesh, GLuint *outTexture) {
  *outMesh = Mesh_LoadModel(modelPath);
  *outTexture = Texture_Load(texturePath);
}

int main() {
  // 1. Init Window
  GLFWwindow *window = initWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE);
  if (!window)
    return -1;

  // 2. Init Input
  Input_Init(window);

  // 3. Init Camera
  Camera camera;
  vec3 startPos = {0.0f, 1.5f, 40.0f};
  vec3 up = {0.0f, 1.0f, 0.0f};
  Camera_Init(&camera, startPos, up, -90.0f, 0.0f);

  // 4. Load Resources
  GLuint shader = Shader_Create("shaders/floor.vert", "shaders/floor.frag");
  GLuint instancedShader =
      Shader_Create("shaders/instanced.vert", "shaders/floor.frag");
  GLuint skyboxShader =
      Shader_Create("shaders/skybox.vert", "shaders/skybox.frag");
  GLuint grassShader =
      Shader_Create("shaders/grass.vert", "shaders/grass.frag");
  GLuint godrayShader =
      Shader_Create("shaders/godray.vert", "shaders/godray.frag");
  GLuint normalMap = Texture_CreateProceduralNormalMap(512, 512);
  GLuint asphaltNormalMap = Texture_CreateNoiseNormalMap(512, 512);
  GLuint grassTexture = Texture_CreateGrassTexture(512, 512);

  // Create Cube Mesh for pathways
  Mesh floorMesh = Mesh_CreateCube(FLOOR_WIDTH, FLOOR_HEIGHT, FLOOR_DEPTH);

  // Create Road Mesh (Asphalt)
  Mesh roadMesh = Mesh_CreateCube(ROAD_WIDTH, ROAD_HEIGHT, ROAD_DEPTH);

  // Create Grass Mesh
  Mesh grassMesh = Mesh_CreateCube(GRASS_WIDTH, GRASS_HEIGHT, GRASS_DEPTH);

  // Load Gazebo Model & Texture
  Mesh gazeboMesh;
  GLuint gazeboTexture;
  LoadModelWithTexture("../materials/gazebo/rgazebo_basic_pbr.fbx",
                       "../materials/gazebo/shaded.png", &gazeboMesh,
                       &gazeboTexture);

  // Create Border Mesh (Curb)
  Mesh borderMesh = Mesh_CreateCube(BORDER_WIDTH, BORDER_HEIGHT, BORDER_DEPTH);

  // Load Bridge Model & Texture
  Mesh bridgeMesh;
  GLuint bridgeTexture;
  LoadModelWithTexture("../materials/bridge/bridge_pbr.fbx",
                       "../materials/bridge/texture_diffuse.png", &bridgeMesh,
                       &bridgeTexture);

  // Load Halfpipe Model & Texture
  Mesh halfpipeMesh;
  GLuint halfpipeTexture;
  LoadModelWithTexture("../materials/halfpipe/halfpipe.fbx",
                       "../materials/halfpipe/halfpipe_texture.png",
                       &halfpipeMesh, &halfpipeTexture);

  // Load Flower Model & Texture
  Mesh flowerMesh;
  GLuint flowerTexture;
  LoadModelWithTexture("../materials/flower/rflower_pbr.fbx",
                       "../materials/flower/shaded.png", &flowerMesh,
                       &flowerTexture);

  // Load White Flower Model & Texture
  Mesh flowerWMesh;
  GLuint flowerWTexture;
  LoadModelWithTexture("../materials/flower_w/rflower_w_pbr.fbx",
                       "../materials/flower_w/shaded.png", &flowerWMesh,
                       &flowerWTexture);

  // Load Skybox Texture
  GLuint skyboxTexture = Texture_Load(
      "../materials/sky/Gemini_Generated_Image_ikqh7oikqh7oikqh.png");
  Mesh skyboxMesh = Mesh_CreateCube(100.0f, 100.0f, 100.0f);

  // Load Hedge Model & Texture
  Mesh hedgeMesh;
  GLuint hedgeTexture;
  LoadModelWithTexture(
      "../materials/hedge/source/hedge-obj/rhedgeTextured.obj",
      "../materials/hedge/source/hedge-obj/hedge-displacement-texture.jpg",
      &hedgeMesh, &hedgeTexture);

  // Create Fence Mesh (Cube)
  Mesh fenceMesh = Mesh_CreateCube(FENCE_WIDTH, FENCE_HEIGHT, FENCE_DEPTH);

  // Create God Ray Mesh (Cylinder)
  // Radius ~0.5, Height ~5.0 (long enough to fade out)
  Mesh godrayMesh = Mesh_CreateCylinder(0.2f, 7.5f, 32);

  // Load Castle Model & Texture
  Mesh castleMesh;
  GLuint castleTexture;
  LoadModelWithTexture("../materials/castle/rcastle.fbx",
                       "../materials/castle/texture_diffuse.png", &castleMesh,
                       &castleTexture);

  // --- Setup Flower Instances ---
  float flowerStartZ = 6.0f;
  float flowerEndZ = 42.5f;
  int zCount = 0;
  for (float z = flowerStartZ; z <= flowerEndZ; z += FLOWER_SPACING_Z) {
    zCount++;
  }
  int totalFlowers = FLOWER_ROWS * zCount * 4; // 4 sections (L/R, Near/Far)
  float *flowerMatrices = (float *)malloc(totalFlowers * 16 * sizeof(float));
  // Allocate extra space for 4 central flowers
  float *flowerWMatrices =
      (float *)malloc((totalFlowers + 4) * 16 * sizeof(float));
  int fIdx = 0;
  int fwIdx = 0;
// Helper macro to add flower to random list
#define ADD_FLOWER(model)                                                      \
  if (rand() % 2 == 0) {                                                       \
    storeMatrix(flowerMatrices, fIdx++, model);                                \
  } else {                                                                     \
    storeMatrix(flowerWMatrices, fwIdx++, model);                              \
  }

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
      ADD_FLOWER(model);
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
      ADD_FLOWER(model);
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
      ADD_FLOWER(model);
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
      ADD_FLOWER(model); // random color
    }
  }

  // --- Add 4 Central White Flowers ---
  float centerXs[] = {-GRASS_OFFSET_X, GRASS_OFFSET_X, -GRASS_OFFSET_X,
                      GRASS_OFFSET_X};
  float centerZs[] = {SECTION_OFFSET_Z, SECTION_OFFSET_Z, -SECTION_OFFSET_Z,
                      -SECTION_OFFSET_Z};

  for (int i = 0; i < 4; i++) {
    mat4 model = identity();
    model = mat4_multiply(translate(centerXs[i], FLOWER_Y_OFFSET, centerZs[i]),
                          model);
    model = mat4_multiply(
        scale(FLOWER_SCALE * 5, 1.5 * FLOWER_SCALE, 5 * FLOWER_SCALE), model);
    model = mat4_multiply(rotate_y(90.0f), model);
    model = mat4_multiply(rotate_x(90.0f), model);
    storeMatrix(flowerWMatrices, fwIdx++, model);
  }

  Mesh_SetupInstanced(&flowerMesh, fIdx, flowerMatrices);
  Mesh_SetupInstanced(&flowerWMesh, fwIdx, flowerWMatrices);
  free(flowerMatrices);
  free(flowerWMatrices);

  // 5. Lighting Config
  vec3 sunDir = {SUN_DIR_X, SUN_DIR_Y, SUN_DIR_Z};
  vec3 sunColor = {SUN_COLOR_R, SUN_COLOR_G, SUN_COLOR_B};
  vec3 skyColor = {SKY_COLOR_R, SKY_COLOR_G, SKY_COLOR_B};
  vec3 groundColor = {GROUND_COLOR_R, GROUND_COLOR_G, GROUND_COLOR_B};

  Shader_Use(grassShader);
  Shader_SetInt(grassShader, "diffuseMap", 1); // Bind to GL_TEXTURE1
  Shader_SetInt(grassShader, "normalMap", 0);  // Bind to GL_TEXTURE0

  Shader_Use(shader);
  Shader_SetInt(shader, "normalMap", 0);
  Shader_SetInt(
      shader, "diffuseMap",
      1); // Texture unit 1 for diffuse .. maybe not work in mac (Metal)
  Shader_SetVec3(shader, "sunDir", sunDir.x, sunDir.y, sunDir.z);
  Shader_SetVec3(shader, "sunColor", sunColor.x, sunColor.y, sunColor.z);
  Shader_SetVec3(shader, "skyColor", skyColor.x, skyColor.y, skyColor.z);
  Shader_SetVec3(shader, "groundColor", groundColor.x, groundColor.y,
                 groundColor.z);

  // --- Setup Hedge Instances ---
  // Hedges at x = -35 and x = 35
  // Similar to flowers, iterate along Z
  float hedgeStartZ = -40.0f; // Start further back
  float hedgeEndZ = 40.0f;    // End further forward
  int hZCount = 0;
  for (float z = hedgeStartZ; z <= hedgeEndZ; z += HEDGE_SPACING_Z) {
    hZCount++;
  }
  int totalHedges = hZCount * 2; // Left and Right
  float *hedgeMatrices = (float *)malloc(totalHedges * 16 * sizeof(float));
  int hIdx = 0;

  // Left Hedges (x = -35)
  for (float z = hedgeStartZ; z <= hedgeEndZ; z += HEDGE_SPACING_Z) {
    if (fabs(z) < 8.0f)
      continue; // Exclude bridge area

    mat4 model = identity();
    model = mat4_multiply(translate(-HEDGE_OFFSET_X, HEDGE_Y_OFFSET, z), model);
    model = mat4_multiply(
        scale(0.7 * HEDGE_SCALE, HEDGE_SCALE * 2.0f, HEDGE_SCALE), model);
    model = mat4_multiply(rotate_y(90.0f), model);
    storeMatrix(hedgeMatrices, hIdx++, model);
  }

  // Right Hedges (x = 35)
  for (float z = hedgeStartZ; z <= hedgeEndZ; z += HEDGE_SPACING_Z) {
    if (fabs(z) < 8.0f)
      continue; // Exclude bridge area

    mat4 model = identity();
    model = mat4_multiply(translate(HEDGE_OFFSET_X, HEDGE_Y_OFFSET, z), model);
    model = mat4_multiply(
        scale(HEDGE_SCALE * 0.7, HEDGE_SCALE * 2.0f, HEDGE_SCALE), model);
    model = mat4_multiply(rotate_y(90.0f), model);
    storeMatrix(hedgeMatrices, hIdx++, model);
  }

  Mesh_SetupInstanced(&hedgeMesh, hIdx, hedgeMatrices);
  free(hedgeMatrices);

  // Water Setup
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  WaterFrameBuffers waterFBOs = WaterFBO_Init(width, height);
  GLuint waterShader =
      Shader_Create("shaders/water.vert", "shaders/water.frag");
  GLuint waterDUDV = Texture_Load("../materials/water/dudv.png");
  // Use existing normalMap for water normal map for now
  Mesh waterMesh = Mesh_CreatePlane(100.0f);
  float waterMoveFactor = 0.0f;

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

    // Store current position before movement for collision logic
    vec3 prevPos = camera.Position;

    // WASD movement (horizontal only)
    if (Input_GetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_FORWARD, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_BACKWARD, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_LEFT, deltaTime);
    if (Input_GetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      Camera_ProcessKeyboard(&camera, CAM_RIGHT, deltaTime);

    // --- Apply World Boundaries (Collision) ---

    // 1. Get Limits
    float currentWalkableWidth = GetWalkableWidth(camera.Position.z);
    float currentGroundHeight =
        GetGroundHeight(camera.Position.x, camera.Position.z);

    // 2. Check Bridge Entry (Prevent Snapping)
    // If we are wider than the new width, we hit a wall.
    if (fabs(camera.Position.x) > currentWalkableWidth) {
      // Check if we entered a narrower zone (e.g. from Road to Bridge)
      float prevWalkableWidth = GetWalkableWidth(prevPos.z);

      if (currentWalkableWidth < prevWalkableWidth) {
        // We are trying to enter a narrower zone but are too wide.
        // Block Z movement (Hit the "bridge wall")
        camera.Position.z = prevPos.z;

        // Re-evaluate width for the reverted Z
        currentWalkableWidth = GetWalkableWidth(camera.Position.z);
        currentGroundHeight =
            GetGroundHeight(camera.Position.x, camera.Position.z);
      }
    }

    // 3. Clamp X (Standard Boundary)
    if (camera.Position.x > currentWalkableWidth)
      camera.Position.x = currentWalkableWidth;
    if (camera.Position.x < -currentWalkableWidth)
      camera.Position.x = -currentWalkableWidth;

    // 4. Clamp Z
    if (camera.Position.z > WORLD_LIMIT_Z)
      camera.Position.z = WORLD_LIMIT_Z;
    if (camera.Position.z < -WORLD_LIMIT_Z)
      camera.Position.z = -WORLD_LIMIT_Z;

    // 5. Enforce Ground Height (No Flying)
    camera.Position.y = currentGroundHeight;

    // Window Size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Water Animation
    waterMoveFactor += 0.1f * deltaTime;
    if (waterMoveFactor > 1.0f)
      waterMoveFactor -= 1.0f;

    // Render 3 Passes
    vec3 savedCameraPos = camera.Position;
    float savedCameraPitch = camera.Pitch;

    for (int pass = 0; pass < 3; pass++) {
      if (pass == 0) {
        WaterFBO_BindReflectionFrameBuffer(&waterFBOs, width, height);
        float distance = 2 * (camera.Position.y - WATER_HEIGHT);
        camera.Position.y -= distance;
        camera.Pitch = -camera.Pitch;
        // Clamp pitch to avoid gimbal lock/singularity when looking straight
        // up/down
        if (camera.Pitch > 80.0f)
          camera.Pitch = 80.0f;
        if (camera.Pitch < -80.0f)
          camera.Pitch = -80.0f;
        Camera_UpdateVectors(&camera);
        glDisable(GL_CULL_FACE);
      } else if (pass == 1) {
        WaterFBO_BindRefractionFrameBuffer(&waterFBOs, width, height);
      } else {
        WaterFBO_UnbindCurrentFrameBuffer(width, height);
      }

      int drawWidth = (pass == 0) ? 1280 : (pass == 1) ? 1280 : width;
      int drawHeight = (pass == 0) ? 720 : (pass == 1) ? 720 : height;
      glViewport(0, 0, drawWidth, drawHeight);

      glClearColor(0.7f, 0.25f, 0.15f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      Shader_Use(shader);

      vec4 plane;
      if (pass == 0)
        plane = (vec4){0.0f, 1.0f, 0.0f, -WATER_HEIGHT + 0.0f};
      else if (pass == 1)
        plane = (vec4){0.0f, -1.0f, 0.0f, WATER_HEIGHT};
      else
        plane = (vec4){0.0f, 1.0f, 0.0f, 10000.0f};
      Shader_SetVec4(shader, "plane", plane.x, plane.y, plane.z, plane.w);

      Shader_Use(grassShader);
      Shader_SetVec4(grassShader, "plane", plane.x, plane.y, plane.z, plane.w);

      Shader_Use(instancedShader);
      Shader_SetVec4(instancedShader, "plane", plane.x, plane.y, plane.z,
                     plane.w);

      Shader_Use(godrayShader);
      Shader_SetVec4(godrayShader, "plane", plane.x, plane.y, plane.z, plane.w);

      Shader_Use(shader);

      mat4 view = Camera_GetViewMatrix(&camera);
      mat4 proj = perspective(1.57f, (float)drawWidth / (float)drawHeight, 0.1f,
                              2000.0f);

      Shader_SetMat4(shader, "view", view.m);
      Shader_SetMat4(shader, "projection", proj.m);
      Shader_SetVec3(shader, "viewPos", camera.Position.x, camera.Position.y,
                     camera.Position.z);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, normalMap);
      Shader_SetInt(shader, "useNormalMap",
                    1); // Enable normal map for floor/road

      // Draw Pathway 1 (Near) - Z = 22.5
      // NOTE: We disable floor rendering in the reflection pass (pass == 0).
      // Since the reflection camera is inverted (underwater looking up),
      // the floor geometry would block the view of the sky and bridge.
      if (pass != 0) {
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
      }

      // --- Draw Asphalt Roads ---
      // NOTE: Disable roads in reflection pass to prevent obstruction.
      if (pass != 0) {
        // Darker color for asphalt
        Shader_SetVec3(shader, "objectColor", 0.2f, 0.2f, 0.22f);
        // Rougher surface for asphalt
        Shader_SetFloat(shader, "shininess", 10.0f);
        Shader_SetFloat(shader, "specularIntensity", 0.1f);

        // Bind Asphalt Normal Map
        glBindTexture(GL_TEXTURE_2D, asphaltNormalMap);

        DrawSymmetricLayer(shader, &roadMesh, ROAD_OFFSET_X, -1.0f,
                           SECTION_OFFSET_Z);
      }

      // --- Draw Road Borders (Curbs) ---
      // Material: Stone Grey
      Shader_SetVec3(shader, "objectColor", 0.7f, 0.7f, 0.7f);
      Shader_SetFloat(shader, "shininess", 32.0f);
      Shader_SetFloat(shader, "specularIntensity", 0.5f);
      Shader_SetInt(shader, "useDiffuseMap", 0); // Ensure no texture

      // Bind default normal map (flat)
      glBindTexture(GL_TEXTURE_2D, normalMap);

      // Draw Inner and Outer Borders symmetrically
      DrawSymmetricLayer(shader, &borderMesh, BORDER_X_INNER, BORDER_Y,
                         SECTION_OFFSET_Z);
      DrawSymmetricLayer(shader, &borderMesh, BORDER_X_OUTER, BORDER_Y,
                         SECTION_OFFSET_Z);

      // --- Draw Outer Floor & Borders (Next to Grass) ---
      // --- Draw Outer Floor & Borders (Next to Grass) ---
      if (pass != 0) {
        // Use Floor Material
        Shader_SetVec3(shader, "objectColor", 0.4f, 0.4f, 0.45f); // Stone Grey
        Shader_SetFloat(shader, "shininess", 32.0f);
        Shader_SetFloat(shader, "specularIntensity", 0.2f);
        Shader_SetInt(shader, "useDiffuseMap", 0);
        Shader_SetInt(shader, "useNormalMap", 1);
        glBindTexture(GL_TEXTURE_2D, normalMap);

        // Draw Outer Floor
        DrawSymmetricLayer(shader, &floorMesh, OUTER_FLOOR_OFFSET_X, -1.0f,
                           SECTION_OFFSET_Z);

        // Use Border Material
        Shader_SetVec3(shader, "objectColor", 0.7f, 0.7f, 0.7f);
        Shader_SetFloat(shader, "shininess", 32.0f);
        Shader_SetFloat(shader, "specularIntensity", 0.5f);

        // Draw Outer Borders
        float outerBorderOffset = 1.925f;
        DrawSymmetricLayer(shader, &borderMesh,
                           OUTER_FLOOR_OFFSET_X - outerBorderOffset, BORDER_Y,
                           SECTION_OFFSET_Z);
        DrawSymmetricLayer(shader, &borderMesh,
                           OUTER_FLOOR_OFFSET_X + outerBorderOffset, BORDER_Y,
                           SECTION_OFFSET_Z);
      }

      // --- Draw Bridge ---
      // Use diffuse map
      Shader_SetInt(shader, "useDiffuseMap", 1);
      // Disable normal map for bridge (it doesn't have one loaded yet, and
      // using road's would be wrong)
      Shader_SetInt(shader, "useNormalMap", 0);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, bridgeTexture);

      mat4 modelBridge = identity();
      modelBridge = mat4_multiply(
          scale(0.72 * BRIDGE_SCALE, 1.1 * BRIDGE_SCALE, 1.0 * BRIDGE_SCALE),
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

      // --- Draw Halfpipe ---
      // NOTE: Disable halfpipe (canal) in reflection pass to prevent
      // obstruction.
      if (pass != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, halfpipeTexture);

        mat4 modelHalfpipe = identity();
        // Scale
        modelHalfpipe =
            mat4_multiply(scale(25 * HALFPIPE_SCALE, 0.6 * HALFPIPE_SCALE,
                                0.6 * HALFPIPE_SCALE),
                          modelHalfpipe);
        // Rotate
        modelHalfpipe = mat4_multiply(rotate_y(90.0f), modelHalfpipe);
        modelHalfpipe = mat4_multiply(rotate_x(180.0f), modelHalfpipe);
        // Position under bridge (lower Y)

        modelHalfpipe = mat4_multiply(
            translate(HALFPIPE_OFFSET_X, HALFPIPE_OFFSET_Y, HALFPIPE_OFFSET_Z),
            modelHalfpipe);

        Shader_SetMat4(shader, "model", modelHalfpipe.m);
        Mesh_Draw(&halfpipeMesh);
      }

      // --- Draw Grass Fields ---
      // NOTE: Disable grass fields in reflection pass to prevent obstruction.
      if (pass != 0) {
        Shader_Use(grassShader);

        // Set View/Projection
        Shader_SetMat4(grassShader, "view", view.m);
        Shader_SetMat4(grassShader, "projection", proj.m);
        Shader_SetVec3(grassShader, "viewPos", camera.Position.x,
                       camera.Position.y, camera.Position.z);

        // Set Lighting Uniforms
        Shader_SetVec3(grassShader, "sunDir", SUN_DIR_X, SUN_DIR_Y, SUN_DIR_Z);
        Shader_SetVec3(grassShader, "sunColor", SUN_COLOR_R, SUN_COLOR_G,
                       SUN_COLOR_B);
        Shader_SetVec3(grassShader, "skyColor", skyColor.x, skyColor.y,
                       skyColor.z);
        Shader_SetVec3(grassShader, "groundColor", groundColor.x, groundColor.y,
                       groundColor.z);

        // Material Properties
        Shader_SetVec3(grassShader, "objectColor", 1.0f, 1.0f, 1.0f);
        Shader_SetInt(grassShader, "useDiffuseMap", 1);
        Shader_SetInt(grassShader, "useNormalMap",
                      1); // Enable normal map for unevenness
        Shader_SetFloat(grassShader, "shininess", 5.0f);
        Shader_SetFloat(grassShader, "specularIntensity", 0.05f);

        // Bind Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
                      asphaltNormalMap); // Use noise normal map for unevenness

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        // Draw Inner Grass
        DrawSymmetricLayer(grassShader, &grassMesh, GRASS_OFFSET_X, -1.0f,
                           SECTION_OFFSET_Z);

        // Draw Outer Grass
        DrawSymmetricLayer(grassShader, &grassMesh, OUTER_GRASS_OFFSET_X, -1.0f,
                           SECTION_OFFSET_Z);
      }

      // Restore Shader for next objects (if any rely on it being active, though
      // usually they switch) But next is Skybox which switches shader. However,
      // we should reset active texture to 0
      glActiveTexture(GL_TEXTURE0);

      // --- Draw Skybox ---
      glDepthFunc(GL_LEQUAL);
      glDisable(GL_CULL_FACE); // Disable culling to see inside the cube
      Shader_Use(skyboxShader);
      // Remove translation from view matrix
      mat4 viewSkybox = view;
      viewSkybox.m[12] = 0.0f;
      viewSkybox.m[13] = 0.0f;
      viewSkybox.m[14] = 0.0f;
      Shader_SetMat4(skyboxShader, "view", viewSkybox.m);
      Shader_SetMat4(skyboxShader, "projection", proj.m);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, skyboxTexture);
      Shader_SetInt(skyboxShader, "skybox", 0);

      Mesh_Draw(&skyboxMesh);
      glEnable(GL_CULL_FACE); // Re-enable culling
      glDepthFunc(GL_LESS);

      // --- Draw Castle ---
      Shader_Use(shader);
      Shader_SetInt(shader, "useDiffuseMap", 1);
      Shader_SetInt(shader, "useNormalMap", 0);
      Shader_SetVec3(shader, "objectColor", 1.0f, 1.0f, 1.0f);
      Shader_SetFloat(shader, "shininess", 10.0f);
      Shader_SetFloat(shader, "specularIntensity", 0.1f);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, castleTexture);

      mat4 modelCastle = identity();
#define CASTLE_SCALE 150.0f
#define CASTLE_OFFSET_X -0.00f
#define CASTLE_OFFSET_Y -0.01f
#define CASTLE_OFFSET_Z -0.85f
      modelCastle = scale(CASTLE_SCALE, CASTLE_SCALE, CASTLE_SCALE);
      modelCastle = mat4_multiply(
          translate(CASTLE_OFFSET_X, CASTLE_OFFSET_Y, CASTLE_OFFSET_Z),
          modelCastle);
      modelCastle = mat4_multiply(rotate_x(90.0f), modelCastle);
      Shader_SetMat4(shader, "model", modelCastle.m);
      Mesh_Draw(&castleMesh);

      // --- Draw Gazebos ---
      Shader_Use(shader);
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
      // We want corners, so let's use offsets close to these values.
      float gOffsetX = 5.0f;
      float gOffsetZ = 15.0f;

      // Relative positions for 4 corners of a field
      float gCornersX[] = {gOffsetX, gOffsetX, -gOffsetX, -gOffsetX};
      float gCornersZ[] = {gOffsetZ, -gOffsetZ, gOffsetZ, -gOffsetZ};

      // Centers of the 4 grass fields
      float fieldCentersX[] = {-GRASS_OFFSET_X, GRASS_OFFSET_X, -GRASS_OFFSET_X,
                               GRASS_OFFSET_X};
      float fieldCentersZ[] = {SECTION_OFFSET_Z, SECTION_OFFSET_Z,
                               -SECTION_OFFSET_Z, -SECTION_OFFSET_Z};

      // Pre-calculate Gazebo Scale and Rotation
      mat4 gazeboSR = identity();
      gazeboSR = mat4_multiply(scale(GAZEBO_SCALE, GAZEBO_SCALE, GAZEBO_SCALE),
                               gazeboSR);
      gazeboSR = mat4_multiply(rotate_x(90.0f), gazeboSR);

      for (int f = 0; f < 4; f++) {   // For each field
        for (int i = 0; i < 4; i++) { // For each corner
          mat4 modelGazebo = identity();
          // Translate: Field Center + Corner Offset
          modelGazebo = mat4_multiply(
              gazeboSR,
              translate(fieldCentersX[f] + gCornersX[i], GAZEBO_Y_OFFSET,
                        fieldCentersZ[f] + gCornersZ[i]));

          Shader_SetMat4(shader, "model", modelGazebo.m);
          Mesh_Draw(&gazeboMesh);
        }
      }

      // --- Draw Flowers (Instanced) ---
      Shader_Use(instancedShader);
      Shader_SetMat4(instancedShader, "view", view.m);
      Shader_SetMat4(instancedShader, "projection", proj.m);

      Shader_SetInt(instancedShader, "diffuseMap", 1); // Texture unit 1
      Shader_SetVec3(instancedShader, "sunDir", sunDir.x, sunDir.y, sunDir.z);
      Shader_SetVec3(instancedShader, "sunColor", sunColor.x, sunColor.y,
                     sunColor.z);
      Shader_SetVec3(instancedShader, "skyColor", skyColor.x, skyColor.y,
                     skyColor.z);
      Shader_SetVec3(instancedShader, "groundColor", groundColor.x,
                     groundColor.y, groundColor.z);
      Shader_SetVec3(instancedShader, "viewPos", camera.Position.x,
                     camera.Position.y, camera.Position.z);

      Shader_SetInt(instancedShader, "useDiffuseMap", 1);
      Shader_SetInt(instancedShader, "useNormalMap", 0);
      Shader_SetFloat(instancedShader, "shininess", 10.0f);
      Shader_SetFloat(instancedShader, "specularIntensity", 0.1f);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, flowerTexture);
      Mesh_DrawInstanced(&flowerMesh, fIdx);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, flowerWTexture);
      Mesh_DrawInstanced(&flowerWMesh, fwIdx);

      // --- Draw Hedges (Instanced) ---
      Shader_Use(instancedShader);
      Shader_SetInt(instancedShader, "useDiffuseMap", 1);
      Shader_SetInt(instancedShader, "useNormalMap", 0);
      Shader_SetFloat(instancedShader, "shininess", 10.0f);
      Shader_SetFloat(instancedShader, "specularIntensity", 0.1f);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, hedgeTexture);

      Mesh_DrawInstanced(&hedgeMesh, hIdx);

      // --- Draw Fences (Between Hedges) ---
      Shader_Use(
          shader); // Use standard shader for fences (no instancing needed for
                   // simple cubes yet, or could instance if many)
      // Simple material for Stone
      Shader_SetVec3(shader, "objectColor", 0.5f, 0.5f, 0.55f); // Stone Grey
      Shader_SetFloat(shader, "shininess", 32.0f);
      Shader_SetFloat(shader, "specularIntensity", 0.2f);
      Shader_SetInt(shader, "useDiffuseMap", 0);
      Shader_SetInt(shader, "useNormalMap", 0); // No normal map for now

      // Iterate through hedge positions to place fences in between
      // Hedge Z loop was: z = hedgeStartZ to hedgeEndZ step HEDGE_SPACING_Z
      // We want to place a fence at z + HEDGE_SPACING_Z / 2.0f
      // But stop before the last hedge
      for (float z = hedgeStartZ; z < hedgeEndZ; z += HEDGE_SPACING_Z) {
        if (fabs(z) < 8.0f)
          continue; // Skip bridge area

        float midZ = z + HEDGE_SPACING_Z / 2.0f;

        // Also check if midZ is in bridge area
        if (fabs(midZ) < 8.0f)
          continue;

        DrawSymmetricLayer(shader, &fenceMesh, HEDGE_OFFSET_X, FENCE_Y_OFFSET,
                           midZ);
      }

      // Reset Active Texture to 0
      glActiveTexture(GL_TEXTURE0);

      // --- Draw God Rays ---
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending
      glDepthMask(GL_FALSE);             // Don't write to depth buffer

      Shader_Use(godrayShader);
      Shader_SetMat4(godrayShader, "view", view.m);
      Shader_SetMat4(godrayShader, "projection", proj.m);
      Shader_SetFloat(godrayShader, "height", 10.0f);
      Shader_SetVec3(godrayShader, "color", 1.0f, 0.9f,
                     0.6f); // Warm light color

      // God Ray Positions (Along the pathway, surrounding flowers)
      // Flowers are at X = +/- 5.0
      // Z range: Start 6.0, End 42.5, Spacing 7.0
      // We place rays slightly outside the flowers (e.g., X = +/- 6.5)
      // and along the same Z intervals.

      float flowerStartZ = 2.7f;
      float flowerEndZ = 42.5f;
      float rayOffsetFromFlower = 0.95f; // Distance from flower center

      for (float z = flowerStartZ; z <= flowerEndZ; z += FLOWER_SPACING_Z) {
        for (int row = 0; row < FLOWER_ROWS; row++) {
          float flowerX = FLOWER_OFFSET_X_START + row * FLOWER_SPACING_X;

          // Two rays per flower (flanking)
          float rayX1 = flowerX - rayOffsetFromFlower;
          float rayX2 = flowerX + rayOffsetFromFlower;

          // --- Ray 1 (Inner side of flower) ---
          // Left Side
          mat4 modelRayLeft1 = identity();
          modelRayLeft1 = mat4_multiply(translate(-rayX1, GODRAY_OFFSET_Y, z),
                                        modelRayLeft1);
          Shader_SetMat4(godrayShader, "model", modelRayLeft1.m);
          Mesh_Draw(&godrayMesh);

          // Right Side
          mat4 modelRayRight1 = identity();
          modelRayRight1 = mat4_multiply(translate(rayX1, GODRAY_OFFSET_Y, z),
                                         modelRayRight1);
          Shader_SetMat4(godrayShader, "model", modelRayRight1.m);
          Mesh_Draw(&godrayMesh);

          // Far Side Left
          mat4 modelRayLeftFar1 = identity();
          modelRayLeftFar1 = mat4_multiply(
              translate(-rayX1, GODRAY_OFFSET_Y, -z), modelRayLeftFar1);
          Shader_SetMat4(godrayShader, "model", modelRayLeftFar1.m);
          Mesh_Draw(&godrayMesh);

          // Far Side Right
          mat4 modelRayRightFar1 = identity();
          modelRayRightFar1 = mat4_multiply(
              translate(rayX1, GODRAY_OFFSET_Y, -z), modelRayRightFar1);
          Shader_SetMat4(godrayShader, "model", modelRayRightFar1.m);
          Mesh_Draw(&godrayMesh);

          // --- Ray 2 (Outer side of flower) ---
          // Left Side
          mat4 modelRayLeft2 = identity();
          modelRayLeft2 = mat4_multiply(translate(-rayX2, GODRAY_OFFSET_Y, z),
                                        modelRayLeft2);
          Shader_SetMat4(godrayShader, "model", modelRayLeft2.m);
          Mesh_Draw(&godrayMesh);

          // Right Side
          mat4 modelRayRight2 = identity();
          modelRayRight2 = mat4_multiply(translate(rayX2, GODRAY_OFFSET_Y, z),
                                         modelRayRight2);
          Shader_SetMat4(godrayShader, "model", modelRayRight2.m);
          Mesh_Draw(&godrayMesh);

          // Far Side Left
          mat4 modelRayLeftFar2 = identity();
          modelRayLeftFar2 = mat4_multiply(
              translate(-rayX2, GODRAY_OFFSET_Y, -z), modelRayLeftFar2);
          Shader_SetMat4(godrayShader, "model", modelRayLeftFar2.m);
          Mesh_Draw(&godrayMesh);

          // Far Side Right
          mat4 modelRayRightFar2 = identity();
          modelRayRightFar2 = mat4_multiply(
              translate(rayX2, GODRAY_OFFSET_Y, -z), modelRayRightFar2);
          Shader_SetMat4(godrayShader, "model", modelRayRightFar2.m);
          Mesh_Draw(&godrayMesh);
        }
      }

      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);

      // Restore Camera
      if (pass == 0) {
        camera.Position = savedCameraPos;
        camera.Pitch = savedCameraPitch;
        Camera_UpdateVectors(&camera);
        glEnable(GL_CULL_FACE);
      }

      // Draw Water (Pass 2)
      if (pass == 2) {
        Shader_Use(waterShader);
        mat4 view = Camera_GetViewMatrix(&camera);
        mat4 proj =
            perspective(1.57f, (float)width / (float)height, 0.1f, 2000.0f);
        mat4 model = identity();
        model = mat4_multiply(scale(1.43f, 1.0f, 0.05f), model);
        model = mat4_multiply(translate(0.0f, WATER_HEIGHT, 0.0f), model);

        Shader_SetMat4(waterShader, "view", view.m);
        Shader_SetMat4(waterShader, "projection", proj.m);
        Shader_SetMat4(waterShader, "model", model.m);
        Shader_SetVec3(waterShader, "cameraPosition", camera.Position.x,
                       camera.Position.y, camera.Position.z);
        Shader_SetVec3(waterShader, "lightPosition", 0.5f, -0.05f, -0.5f);
        Shader_SetVec3(waterShader, "lightColor", 1.0f, 0.6f, 0.4f);
        Shader_SetVec3(waterShader, "skyColor", skyColor.x, skyColor.y,
                       skyColor.z);
        Shader_SetFloat(waterShader, "moveFactor", waterMoveFactor);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterFBOs.reflectionTexture);
        Shader_SetInt(waterShader, "reflectionTexture", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, waterFBOs.refractionTexture);
        Shader_SetInt(waterShader, "refractionTexture", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, waterDUDV);
        Shader_SetInt(waterShader, "dudvMap", 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        Shader_SetInt(waterShader, "normalMap", 3);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, waterFBOs.refractionDepthTexture);
        Shader_SetInt(waterShader, "depthMap", 4);

        Mesh_Draw(&waterMesh);
      }
    } // End for loop

    glfwSwapBuffers(window);
    glfwPollEvents();

    // DEBUG: Player Position (Easy to remove)
    printf("Player Pos: %.2f, %.2f, %.2f\n", camera.Position.x,
           camera.Position.y, camera.Position.z);
    static int frameCount = 0;
    if (frameCount++ % 60 == 0) { // Print once every 60 frames to avoid spam
      printf("Player Pos: %.2f, %.2f, %.2f\n", camera.Position.x,
             camera.Position.y, camera.Position.z);
    }
  }

  WaterFBO_CleanUp(&waterFBOs);
  glfwTerminate();
  return 0;
}
