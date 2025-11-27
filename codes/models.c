#include "models.h"
#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Display List IDs
GLuint listGazebo = 0;
GLuint listTree = 0;
GLuint listBridge = 0;
GLuint listLampPost = 0;
GLuint listFlowerRed = 0;
GLuint listFlowerWhite = 0;

// Helper Functions
void m_drawCylinder(float baseRadius, float topRadius, float height,
                    int slices) {
  GLUquadricObj *quad = gluNewQuadric();
  gluCylinder(quad, baseRadius, topRadius, height, slices, 1);
  gluDeleteQuadric(quad);
}

void m_drawDisk(float innerRadius, float outerRadius, int slices) {
  GLUquadricObj *quad = gluNewQuadric();
  gluDisk(quad, innerRadius, outerRadius, slices, 1);
  gluDeleteQuadric(quad);
}

void m_drawSphere(float radius, int slices, int stacks) {
  glutSolidSphere(radius, slices, stacks);
}

// --- Model Generation ---

void genGazebo() {
  glNewList(listGazebo, GL_COMPILE);

  // 1. Base Steps (3 Layers of White Marble)
  GLfloat mat_marble[] = {0.9f, 0.9f, 0.95f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_marble);

  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  m_drawCylinder(1.5, 1.5, 0.1, 16);
  glTranslatef(0.0f, 0.0f, 0.1f);
  m_drawDisk(0.0, 1.5, 16);

  m_drawCylinder(1.4, 1.4, 0.1, 16);
  glTranslatef(0.0f, 0.0f, 0.1f);
  m_drawDisk(0.0, 1.4, 16);

  m_drawCylinder(1.3, 1.3, 0.1, 16);
  glTranslatef(0.0f, 0.0f, 0.1f);
  m_drawDisk(0.0, 1.3, 16);
  glPopMatrix();

  // 2. Pillars (6 pillars)
  GLfloat mat_pillar[] = {0.95f, 0.95f, 0.9f, 1.0f};
  GLfloat mat_gold[] = {0.8f, 0.6f, 0.1f, 1.0f};

  for (int i = 0; i < 6; i++) {
    glPushMatrix();
    float angle = i * 60.0f * M_PI / 180.0f;
    float r = 1.1f;
    glTranslatef(r * cos(angle), 0.3f, r * sin(angle));
    glRotatef(-90, 1, 0, 0);

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
    m_drawCylinder(0.12, 0.1, 0.1, 8);

    glTranslatef(0.0f, 0.0f, 0.1f);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_pillar);
    m_drawCylinder(0.1, 0.1, 2.0, 8);

    glTranslatef(0.0f, 0.0f, 2.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
    m_drawCylinder(0.1, 0.12, 0.1, 8);
    glPopMatrix();

    // Railings
    glPushMatrix();
    float nextAngle = (i + 1) * 60.0f * M_PI / 180.0f;
    float x1 = r * cos(angle);
    float z1 = r * sin(angle);
    float x2 = r * cos(nextAngle);
    float z2 = r * sin(nextAngle);
    float mx = (x1 + x2) / 2.0f;
    float mz = (z1 + z2) / 2.0f;
    float dist = sqrt(pow(x2 - x1, 2) + pow(z2 - z1, 2));
    float rot = atan2(z2 - z1, x2 - x1) * 180.0f / M_PI;

    glTranslatef(mx, 0.8f, mz);
    glRotatef(-rot, 0, 1, 0);

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0.0f, 0.0f, -dist / 2);
    m_drawCylinder(0.03, 0.03, dist, 4);
    glPopMatrix();

    glTranslatef(0.0f, -0.4f, 0.0f);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0.0f, 0.0f, -dist / 2);
    m_drawCylinder(0.03, 0.03, dist, 4);
    glPopMatrix();
    glPopMatrix();
  }

  // 3. Roof
  glPushMatrix();
  glTranslatef(0.0f, 2.5f, 0.0f);
  GLfloat mat_roof[] = {0.8f, 0.1f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_roof);

  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  m_drawCylinder(1.8, 1.4, 0.3, 16);
  glTranslatef(0.0f, 0.0f, 0.3f);
  m_drawCylinder(1.4, 1.0, 0.3, 16);
  glTranslatef(0.0f, 0.0f, 0.3f);
  m_drawCylinder(1.0, 0.6, 0.3, 16);
  glTranslatef(0.0f, 0.0f, 0.3f);
  m_drawCylinder(0.6, 0.0, 0.5, 16);
  glPopMatrix();

  glTranslatef(0.0f, 1.4f, 0.0f);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
  m_drawSphere(0.2, 8, 8);
  glPopMatrix();

  glEndList();
}

void genTree() {
  glNewList(listTree, GL_COMPILE);

  GLfloat mat_wood[] = {0.3f, 0.2f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_wood);
  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  m_drawCylinder(0.2, 0.1, 1.5, 6);
  glPopMatrix();

  GLfloat mat_leaves[] = {0.0f, 0.4f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_leaves);

  glPushMatrix();
  glTranslatef(0.0f, 2.0f, 0.0f);
  m_drawSphere(1.0, 8, 8);
  glPopMatrix();

  for (int i = 0; i < 4; i++) {
    glPushMatrix();
    float angle = i * 90.0f * M_PI / 180.0f;
    glTranslatef(0.6f * cos(angle), 1.5f, 0.6f * sin(angle));
    m_drawSphere(0.7, 6, 6);
    glPopMatrix();
  }

  glEndList();
}

void genBridge() {
  glNewList(listBridge, GL_COMPILE);

  GLfloat mat_red[] = {0.8f, 0.2f, 0.1f, 1.0f};
  GLfloat mat_gold[] = {0.8f, 0.6f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_red);

  // Art Nouveau Style: Curved organic shape
  int segments = 24; // More segments for smoothness
  float width = 2.5f;
  float length = 6.0f;
  float height = 1.5f;

  glBegin(GL_QUADS);
  for (int i = 0; i < segments; i++) {
    float t1 = (float)i / segments;
    float t2 = (float)(i + 1) / segments;
    float x1 = (t1 - 0.5f) * length;
    float y1 = 4 * height * t1 * (1.0f - t1); // Parabola
    float x2 = (t2 - 0.5f) * length;
    float y2 = 4 * height * t2 * (1.0f - t2);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x1, y1, -width / 2);
    glVertex3f(x2, y2, -width / 2);
    glVertex3f(x2, y2, width / 2);
    glVertex3f(x1, y1, width / 2);

    // Side walls (Thicker)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(x1, y1 - 0.2f, -width / 2);
    glVertex3f(x2, y2 - 0.2f, -width / 2);
    glVertex3f(x2, y2 + 0.5f, -width / 2); // Higher railing
    glVertex3f(x1, y1 + 0.5f, -width / 2);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x1, y1 - 0.2f, width / 2);
    glVertex3f(x2, y2 - 0.2f, width / 2);
    glVertex3f(x2, y2 + 0.5f, width / 2);
    glVertex3f(x1, y1 + 0.5f, width / 2);
  }
  glEnd();

  // Gold Accents (Orbs on railings)
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
  for (int i = 0; i <= segments; i += 4) { // Every few segments
    float t = (float)i / segments;
    float x = (t - 0.5f) * length;
    float y = 4 * height * t * (1.0f - t) + 0.5f;
    glPushMatrix();
    glTranslatef(x, y, -width / 2);
    m_drawSphere(0.15, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x, y, width / 2);
    m_drawSphere(0.15, 8, 8);
    glPopMatrix();
  }
  glEndList();
}

void genLampPost() {
  glNewList(listLampPost, GL_COMPILE);

  GLfloat mat_iron[] = {0.1f, 0.1f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_iron);
  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  m_drawCylinder(0.1, 0.05, 2.0, 6);
  glPopMatrix();

  glTranslatef(0.0f, 2.0f, 0.0f);
  GLfloat mat_gold[] = {0.8f, 0.7f, 0.2f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
  glutSolidCube(0.4);

  GLfloat mat_light[] = {1.0f, 1.0f, 0.8f, 1.0f};
  GLfloat mat_emissive[] = {0.8f, 0.8f, 0.5f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_light);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emissive);
  m_drawSphere(0.15, 6, 6);

  GLfloat no_emit[] = {0.0f, 0.0f, 0.0f, 1.0f};
  glMaterialfv(GL_FRONT, GL_EMISSION, no_emit);

  glEndList();
}

void genFlower(int isRed) {
  GLuint list = isRed ? listFlowerRed : listFlowerWhite;
  glNewList(list, GL_COMPILE);

  // Stem
  GLfloat mat_green[] = {0.1f, 0.6f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_green);
  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  m_drawCylinder(0.02, 0.02, 0.3, 4);
  glPopMatrix();

  // Petals
  if (isRed) {
    GLfloat col[] = {0.9f, 0.1f, 0.1f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, col);
  } else {
    GLfloat col[] = {0.95f, 0.95f, 0.95f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, col);
  }

  glTranslatef(0.0f, 0.3f, 0.0f);
  for (int i = 0; i < 5; i++) {
    glPushMatrix();
    glRotatef(i * 72, 0, 1, 0);
    glRotatef(45, 1, 0, 0);
    glTranslatef(0.0f, 0.0f, 0.05f);
    glScalef(0.1f, 0.02f, 0.15f);
    glutSolidSphere(1.0, 4, 4);
    glPopMatrix();
  }

  // Center
  GLfloat mat_yellow[] = {1.0f, 0.8f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_yellow);
  m_drawSphere(0.05, 4, 4);

  glEndList();
}

GLuint listPalace = 0;
GLuint listGlowingPillar = 0;

void genPalace() {
  glNewList(listPalace, GL_COMPILE);

  GLfloat mat_palace[] = {0.6f, 0.1f, 0.1f, 1.0f}; // Dark Red
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_palace);

  // Massive Cylinder Background
  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  m_drawCylinder(20.0, 20.0, 30.0, 32); // Huge
  glPopMatrix();

  glEndList();
}

void genGlowingPillar() {
  glNewList(listGlowingPillar, GL_COMPILE);

  GLfloat mat_glow[] = {1.0f, 0.8f, 0.6f, 1.0f};
  GLfloat mat_emit[] = {1.0f, 0.6f, 0.2f, 1.0f}; // Strong Orange Emission

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_glow);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emit);

  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  m_drawCylinder(0.3, 0.3, 6.0, 16); // Tall pillar
  glPopMatrix();

  // Reset Emission
  GLfloat no_emit[] = {0.0f, 0.0f, 0.0f, 1.0f};
  glMaterialfv(GL_FRONT, GL_EMISSION, no_emit);

  glEndList();
}

// --- Public API ---

void initModels() {
  if (listGazebo == 0) {
    listGazebo = glGenLists(8); // Increased from 6 to 8
    listTree = listGazebo + 1;
    listBridge = listGazebo + 2;
    listLampPost = listGazebo + 3;
    listFlowerRed = listGazebo + 4;
    listFlowerWhite = listGazebo + 5;
    listPalace = listGazebo + 6;        // New
    listGlowingPillar = listGazebo + 7; // New

    genGazebo();
    genTree();
    genBridge();
    genLampPost();
    genFlower(1);       // Red
    genFlower(0);       // White
    genPalace();        // New
    genGlowingPillar(); // New
  }
}

void drawGazeboModel() { glCallList(listGazebo); }
void drawTreeModel() { glCallList(listTree); }
void drawBridgeModel() { glCallList(listBridge); }
void drawLampPostModel() { glCallList(listLampPost); }
void drawFlowerModel(float r, float g, float b) {
  if (r > 0.5f)
    glCallList(listFlowerRed);
  else
    glCallList(listFlowerWhite);
}
void drawPalaceModel() { glCallList(listPalace); }               // New
void drawGlowingPillarModel() { glCallList(listGlowingPillar); } // New

void drawSkyDomeModel(float radius) {
  glDisable(GL_LIGHTING);
  int slices = 32;
  int stacks = 16;
  for (int i = 0; i < stacks; i++) {
    float lat0 = M_PI * (-0.5 + (float)(i) / stacks);
    float z0 = sin(lat0);
    float zr0 = cos(lat0);
    float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);
    float z1 = sin(lat1);
    float zr1 = cos(lat1);

    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= slices; j++) {
      float lng = 2 * M_PI * (float)(j - 1) / slices;
      float x = cos(lng);
      float y = sin(lng);
      float t = (z0 + 1.0f) * 0.5f;
      glColor3f(0.8f - t * 0.8f, 0.4f - t * 0.4f, 0.2f + t * 0.2f);
      glVertex3f(x * zr0 * radius, z0 * radius, y * zr0 * radius);
      t = (z1 + 1.0f) * 0.5f;
      glColor3f(0.8f - t * 0.8f, 0.4f - t * 0.4f, 0.2f + t * 0.2f);
      glVertex3f(x * zr1 * radius, z1 * radius, y * zr1 * radius);
    }
    glEnd();
  }
  glEnable(GL_LIGHTING);
}
