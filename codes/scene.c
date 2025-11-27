#include "scene.h"
#include "map_data.h"
#include "models.h"
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Global time variable for animation
float sceneTime = 0.0f;

void updateSceneTime() { sceneTime += 0.05f; }

void setupLighting() {
  // 1. Global Ambient (Brighter to avoid pitch black)
  GLfloat global_ambient[] = {0.3f, 0.25f, 0.35f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

  // 2. Main Sun/Moon (Directional - Sunset)
  // Warm, strong directional light from the west/horizon
  GLfloat sun_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
  GLfloat sun_diffuse[] = {1.0f, 0.8f, 0.6f, 1.0f}; // Golden Hour
  GLfloat sun_specular[] = {1.0f, 0.9f, 0.8f, 1.0f};
  GLfloat sun_position[] = {-50.0f, 30.0f, -50.0f, 0.0f}; // Directional (w=0)

  glLightfv(GL_LIGHT0, GL_AMBIENT, sun_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sun_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
  glEnable(GL_LIGHT0);

  // 3. Point Lights (Lamps) - Up to 6 lights
  // We place them at strategic locations along the path/bridge.
  // Map Scale is 4.0. Center is (0,0).
  // Bridge is roughly at Z=0 (Rows 8-13 are centered around Z=0 if we offset
  // correctly). Let's place lights along the X axis (Path) and near the Bridge.

  // Lamp Color: Warm Candle/Tungsten
  GLfloat lamp_diffuse[] = {1.0f, 0.6f, 0.2f, 1.0f};
  GLfloat lamp_specular[] = {1.0f, 0.8f, 0.4f, 1.0f};
  GLfloat lamp_attenuation_const = 1.0f;
  GLfloat lamp_attenuation_linear = 0.05f;
  GLfloat lamp_attenuation_quad = 0.005f;

  // Positions (Approximate based on map layout and 4.0 scale)
  // Path is roughly X axis? No, map is Z rows.
  // Let's place lights at:
  // Light 1: Near start of bridge (Left)
  // Light 2: Near start of bridge (Right)
  // Light 3: Far side bridge (Left)
  // Light 4: Far side bridge (Right)
  // Light 5: Path approach
  // Light 6: Path exit

  float y_h = 4.0f; // Height of light (Lamp post height)
  float scale = 4.0f;

  float light_positions[6][4] = {
      {-10.0f * scale, y_h, -2.0f * scale, 1.0f}, // Near Left
      {-10.0f * scale, y_h, 2.0f * scale, 1.0f},  // Near Right
      {10.0f * scale, y_h, -2.0f * scale, 1.0f},  // Far Left
      {10.0f * scale, y_h, 2.0f * scale, 1.0f},   // Far Right
      {-20.0f * scale, y_h, 0.0f * scale, 1.0f},  // Approach
      {20.0f * scale, y_h, 0.0f * scale, 1.0f}    // Exit
  };

  for (int i = 0; i < 6; i++) {
    int light_id = GL_LIGHT1 + i;
    glLightfv(light_id, GL_DIFFUSE, lamp_diffuse);
    glLightfv(light_id, GL_SPECULAR, lamp_specular);
    glLightfv(light_id, GL_POSITION, light_positions[i]);

    glLightf(light_id, GL_CONSTANT_ATTENUATION, lamp_attenuation_const);
    glLightf(light_id, GL_LINEAR_ATTENUATION, lamp_attenuation_linear);
    glLightf(light_id, GL_QUADRATIC_ATTENUATION, lamp_attenuation_quad);

    glEnable(light_id);
  }

  // Fog
  GLfloat fogColor[] = {0.2f, 0.15f, 0.3f, 1.0f}; // Lighter Twilight Purple
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_START, 30.0f); // Start further away
  glFogf(GL_FOG_END, 200.0f);  // End further away
  glHint(GL_FOG_HINT, GL_NICEST);
}

void drawGroundTile(float x, float z, int type) {
  glPushMatrix();
  glTranslatef(x, 0.0f, z);

  GLfloat mat_ambient[4];
  GLfloat mat_diffuse[4];
  GLfloat mat_specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
  GLfloat shininess = 10.0f;

  switch (type) {
  case 1: // Tiles
    mat_ambient[0] = 0.2f;
    mat_ambient[1] = 0.2f;
    mat_ambient[2] = 0.25f;
    mat_ambient[3] = 1.0f;
    mat_diffuse[0] = 0.5f;
    mat_diffuse[1] = 0.5f;
    mat_diffuse[2] = 0.55f;
    mat_diffuse[3] = 1.0f;
    break;
  case 2: // Asphalt
    mat_ambient[0] = 0.3f;
    mat_ambient[1] = 0.28f;
    mat_ambient[2] = 0.25f;
    mat_ambient[3] = 1.0f;
    mat_diffuse[0] = 0.6f;
    mat_diffuse[1] = 0.58f;
    mat_diffuse[2] = 0.55f;
    mat_diffuse[3] = 1.0f;
    break;
  case 3: // Grass
    mat_ambient[0] = 0.05f;
    mat_ambient[1] = 0.25f;
    mat_ambient[2] = 0.05f;
    mat_ambient[3] = 1.0f;
    mat_diffuse[0] = 0.1f;
    mat_diffuse[1] = 0.6f;
    mat_diffuse[2] = 0.1f;
    mat_diffuse[3] = 1.0f;
    break;
  case 5: // Riverbed (Mud/Dark)
    mat_ambient[0] = 0.1f;
    mat_ambient[1] = 0.1f;
    mat_ambient[2] = 0.1f;
    mat_ambient[3] = 1.0f;
    mat_diffuse[0] = 0.2f;
    mat_diffuse[1] = 0.15f;
    mat_diffuse[2] = 0.1f;
    mat_diffuse[3] = 1.0f;
    break;
  default:
    mat_ambient[0] = 0.5f;
    mat_ambient[1] = 0.5f;
    mat_ambient[2] = 0.5f;
    mat_ambient[3] = 1.0f;
    mat_diffuse[0] = 0.5f;
    mat_diffuse[1] = 0.5f;
    mat_diffuse[2] = 0.5f;
    mat_diffuse[3] = 1.0f;
    break;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);

  int sub = 2;
  float step = 1.0f / sub;
  glNormal3f(0.0f, 1.0f, 0.0f);

  for (int i = 0; i < sub; i++) {
    for (int j = 0; j < sub; j++) {
      float x0 = -0.5f + i * step;
      float z0 = -0.5f + j * step;
      glBegin(GL_QUADS);
      glVertex3f(x0, 0.0f, z0);
      glVertex3f(x0, 0.0f, z0 + step);
      glVertex3f(x0 + step, 0.0f, z0 + step);
      glVertex3f(x0 + step, 0.0f, z0);
      glEnd();
    }
  }
  glPopMatrix();
}

void drawWaterSurface(float x, float z) {
  glPushMatrix();
  glTranslatef(x, -0.5f, z); // Water level is lower (-0.5)

  GLfloat mat_water_diff[] = {0.1f, 0.2f, 0.5f, 0.8f};
  GLfloat mat_water_spec[] = {0.8f, 0.8f, 0.9f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_water_diff);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_water_spec);
  glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);

  int sub = 4;
  float step = 1.0f / sub;

  glBegin(GL_QUADS);
  for (int i = 0; i < sub; i++) {
    for (int j = 0; j < sub; j++) {
      float x0 = -0.5f + i * step;
      float z0 = -0.5f + j * step;
      float x1 = x0 + step;
      float z1 = z0 + step;

      float wx0 = x + x0;
      float wz0 = z + z0;
      float wx1 = x + x1;
      float wz1 = z + z1;

      float y00 = 0.05f * sin(wx0 * 2.0f + sceneTime) *
                  cos(wz0 * 1.5f + sceneTime * 0.8f);
      float y01 = 0.05f * sin(wx0 * 2.0f + sceneTime) *
                  cos(wz1 * 1.5f + sceneTime * 0.8f);
      float y11 = 0.05f * sin(wx1 * 2.0f + sceneTime) *
                  cos(wz1 * 1.5f + sceneTime * 0.8f);
      float y10 = 0.05f * sin(wx1 * 2.0f + sceneTime) *
                  cos(wz0 * 1.5f + sceneTime * 0.8f);

      glNormal3f(0.0f, 1.0f, 0.0f);
      glVertex3f(x0, y00, z0);
      glVertex3f(x0, y01, z1);
      glVertex3f(x1, y11, z1);
      glVertex3f(x1, y10, z0);
    }
  }
  glEnd();
  glPopMatrix();
}

void drawRiverBank(float x, float z, int direction) {
  // Direction: 0=North(-Z), 1=South(+Z), 2=West(-X), 3=East(+X)
  glPushMatrix();
  glTranslatef(x, 0.0f, z);

  GLfloat mat_bank[] = {0.3f, 0.25f, 0.2f, 1.0f}; // Dark soil
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_bank);

  glBegin(GL_QUADS);
  glNormal3f(0.0f, 0.0f, 1.0f); // Normals will vary but flat shading is fine

  if (direction == 0) { // North neighbor is water, draw North face
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, -1.0f, -0.5f);
    glVertex3f(-0.5f, -1.0f, -0.5f);
  } else if (direction == 1) { // South
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, -1.0f, 0.5f);
    glVertex3f(-0.5f, -1.0f, 0.5f);
  } else if (direction == 2) { // West
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, -1.0f, 0.5f);
    glVertex3f(-0.5f, -1.0f, -0.5f);
  } else if (direction == 3) { // East
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, -1.0f, 0.5f);
    glVertex3f(0.5f, -1.0f, -0.5f);
  }
  glEnd();
  glPopMatrix();
}

#include "shader.h"

GLuint shaderProgram = 0;

void drawObjects(float mapScale, int isReflection) {
  float offsetX = -(MAP_WIDTH / 2.0f);
  float offsetZ = -(MAP_HEIGHT / 2.0f);

  // Draw Palace (Background)
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -60.0f * mapScale);
  glScalef(mapScale, mapScale, mapScale);
  drawPalaceModel();
  glPopMatrix();

  for (int z = 0; z < MAP_HEIGHT; z++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      char cell = map_layout[z][x];
      float wx = (x + offsetX) * mapScale;
      float wz = (z + offsetZ) * mapScale;

      glPushMatrix();
      glTranslatef(wx, 0.0f, wz);
      glScalef(mapScale, 1.0f, mapScale);

      // Terrain Logic (Skip Water)
      if (cell == '~') {
        // Riverbed
        glPushMatrix();
        glTranslatef(0.0f, -1.0f, 0.0f);
        drawGroundTile(0, 0, 5);
        glPopMatrix();
        // Skip Water Surface here
      } else {
        // Normal Ground
        if (cell == '#')
          drawGroundTile(0, 0, 1);
        else if (cell == '=')
          drawGroundTile(0, 0, 2);
        else
          drawGroundTile(0, 0, 3);

        // River Banks
        if (z > 0 && map_layout[z - 1][x] == '~')
          drawRiverBank(0, 0, 0);
        if (z < MAP_HEIGHT - 1 && map_layout[z + 1][x] == '~')
          drawRiverBank(0, 0, 1);
        if (x > 0 && map_layout[z][x - 1] == '~')
          drawRiverBank(0, 0, 2);
        if (x < MAP_WIDTH - 1 && map_layout[z][x + 1] == '~')
          drawRiverBank(0, 0, 3);
      }
      glPopMatrix();

      // Objects
      glPushMatrix();
      glTranslatef(wx, 0.0f, wz);

      // Reverse culling for reflection if needed, but glScalef(1,-1,1) handles
      // it usually if we change FrontFace However, for simple objects, it might
      // be fine.

      if (cell == 'G') {
        drawGazeboModel();
      } else if (cell == 'T') {
        drawTreeModel();

        int isNearPath = 0;
        if (x > 0 && map_layout[z][x - 1] == '=')
          isNearPath = 1;
        if (x < MAP_WIDTH - 1 && map_layout[z][x + 1] == '=')
          isNearPath = 1;
        if (z > 0 && map_layout[z - 1][x] == '=')
          isNearPath = 1;
        if (z < MAP_HEIGHT - 1 && map_layout[z + 1][x] == '=')
          isNearPath = 1;

        if (isNearPath) {
          glPushMatrix();
          glTranslatef(0.0f, 0.0f, 1.5f);
          drawLampPostModel();
          glPopMatrix();
        }
      } else if (cell == 'B') {
        if (x == 20 || x == 21) {
          if (x > 0 && map_layout[z][x - 1] == 'B' &&
              map_layout[z][x + 1] == 'B') {
            if (map_layout[z][x - 2] == 'B' && map_layout[z][x + 2] == 'B') {
              glPushMatrix();
              glScalef(mapScale, mapScale, mapScale);
              glRotatef(90, 0, 1, 0);
              drawBridgeModel();
              glPopMatrix();

              // Pillars
              glPushMatrix();
              glTranslatef(-3.0f, 0.0f, -1.5f);
              drawGlowingPillarModel();
              glPopMatrix();
              glPushMatrix();
              glTranslatef(-3.0f, 0.0f, 1.5f);
              drawGlowingPillarModel();
              glPopMatrix();
              glPushMatrix();
              glTranslatef(3.0f, 0.0f, -1.5f);
              drawGlowingPillarModel();
              glPopMatrix();
              glPushMatrix();
              glTranslatef(3.0f, 0.0f, 1.5f);
              drawGlowingPillarModel();
              glPopMatrix();
            }
          }
        }
      } else if (cell == 'R') {
        for (int k = 0; k < 3; k++) {
          glPushMatrix();
          float fx = ((k * 123) % 10 - 5) / 10.0f * mapScale * 0.4f;
          float fz = ((k * 456) % 10 - 5) / 10.0f * mapScale * 0.4f;
          glTranslatef(fx, 0.0f, fz);
          drawFlowerModel(1.0f, 0.0f, 0.0f);
          glPopMatrix();
        }
      } else if (cell == 'w') {
        for (int k = 0; k < 3; k++) {
          glPushMatrix();
          float fx = ((k * 789) % 10 - 5) / 10.0f * mapScale * 0.4f;
          float fz = ((k * 321) % 10 - 5) / 10.0f * mapScale * 0.4f;
          glTranslatef(fx, 0.0f, fz);
          drawFlowerModel(0.0f, 0.0f, 0.0f);
          glPopMatrix();
        }
      }
      glPopMatrix();
    }
  }
}

void drawWater(float mapScale) {
  float offsetX = -(MAP_WIDTH / 2.0f);
  float offsetZ = -(MAP_HEIGHT / 2.0f);

  for (int z = 0; z < MAP_HEIGHT; z++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map_layout[z][x] == '~') {
        float wx = (x + offsetX) * mapScale;
        float wz = (z + offsetZ) * mapScale;
        glPushMatrix();
        glTranslatef(wx, 0.0f, wz);
        glScalef(mapScale, 1.0f, mapScale);
        drawWaterSurface(0, 0);
        glPopMatrix();
      }
    }
  }
}

void drawScene() {
  static int firstRun = 1;
  if (firstRun) {
    initModels();
    shaderProgram = loadShader("basic.vert", "basic.frag");
    firstRun = 0;
  }

  // Ensure lighting is set up every frame (for correct view space positions)
  setupLighting();

  float mapScale = 4.0f;

  // 1. Draw Sky (Always first, no reflection needed usually or simple
  // reflection) Actually sky should be reflected too.

  // --- REFLECTION PASS ---
  // Clear Stencil
  glClear(GL_STENCIL_BUFFER_BIT);
  glEnable(GL_STENCIL_TEST);

  // Draw Water to Stencil (Mask)
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);

  drawWater(mapScale);

  // Draw Reflection where Stencil == 1
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  glPushMatrix();
  glScalef(1.0f, -1.0f, 1.0f); // Invert Y
  glFrontFace(GL_CW);          // Invert winding order

  // Draw Sky in Reflection
  glDisable(GL_FOG); // Disable Fog for Sky
  if (shaderProgram) {
    glUseProgram(shaderProgram);
    GLint loc = glGetUniformLocation(shaderProgram, "objectType");
    glUniform1i(loc, 1); // 1 = Sky
  }
  drawSkyDomeModel(100.0f * mapScale);
  glEnable(GL_FOG); // Re-enable Fog

  if (shaderProgram) {
    glUseProgram(shaderProgram);
    GLint loc = glGetUniformLocation(shaderProgram, "objectType");
    glUniform1i(loc, 0); // General
  }
  drawObjects(mapScale, 1);
  glUseProgram(0);

  glFrontFace(GL_CCW); // Restore winding
  glPopMatrix();

  glDisable(GL_STENCIL_TEST);

  // --- NORMAL PASS ---
  // Draw Sky
  glDisable(GL_FOG); // Disable Fog for Sky
  if (shaderProgram) {
    glUseProgram(shaderProgram);
    GLint loc = glGetUniformLocation(shaderProgram, "objectType");
    glUniform1i(loc, 1); // 1 = Sky
  }
  drawSkyDomeModel(100.0f * mapScale);
  glEnable(GL_FOG); // Re-enable Fog

  // Draw Objects
  if (shaderProgram) {
    glUseProgram(shaderProgram);
    GLint loc = glGetUniformLocation(shaderProgram, "objectType");
    glUniform1i(loc, 0); // General
  }
  drawObjects(mapScale, 0);

  // Draw Water Surface (Blending)
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (shaderProgram) {
    GLint loc = glGetUniformLocation(shaderProgram, "objectType");
    glUniform1i(loc, 2); // Water
  }
  // Set water alpha to semi-transparent to show reflection
  // We need to modify drawWaterSurface or set a uniform/color
  // Assuming drawWaterSurface uses a material, we can override alpha?
  // Or just draw it.
  drawWater(mapScale);

  glDisable(GL_BLEND);
  glUseProgram(0);
}
