#include "shapes.h"
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

void drawCylinder(float baseRadius, float topRadius, float height, int slices) {
  GLUquadricObj *quad = gluNewQuadric();
  gluCylinder(quad, baseRadius, topRadius, height, slices, 1);
  gluDeleteQuadric(quad);
}

void drawDisk(float innerRadius, float outerRadius, int slices) {
  GLUquadricObj *quad = gluNewQuadric();
  gluDisk(quad, innerRadius, outerRadius, slices, 1);
  gluDeleteQuadric(quad);
}

void drawSphere(float radius, int slices, int stacks) {
  glutSolidSphere(radius, slices, stacks);
}

// --- Procedural Generation Functions ---

void generateGazebo() {
  glNewList(listGazebo, GL_COMPILE);

  // 1. Base Steps (3 Layers of White Marble)
  GLfloat mat_marble[] = {0.9f, 0.9f, 0.95f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_marble);

  glPushMatrix();
  glRotatef(-90, 1, 0, 0);

  // Step 1 (Bottom, widest)
  drawCylinder(1.5, 1.5, 0.1, 8);
  glTranslatef(0.0f, 0.0f, 0.1f);
  drawDisk(0.0, 1.5, 8);

  // Step 2
  drawCylinder(1.4, 1.4, 0.1, 8);
  glTranslatef(0.0f, 0.0f, 0.1f);
  drawDisk(0.0, 1.4, 8);

  // Step 3 (Floor)
  drawCylinder(1.3, 1.3, 0.1, 8);
  glTranslatef(0.0f, 0.0f, 0.1f);
  drawDisk(0.0, 1.3, 8);

  glPopMatrix();

  // 2. Pillars (6 pillars, Gold accents)
  GLfloat mat_pillar[] = {0.95f, 0.95f, 0.9f, 1.0f};
  GLfloat mat_gold[] = {0.8f, 0.6f, 0.1f, 1.0f};

  for (int i = 0; i < 6; i++) {
    glPushMatrix();
    float angle = i * 60.0f * M_PI / 180.0f;
    float r = 1.1f;
    glTranslatef(r * cos(angle), 0.3f, r * sin(angle));
    glRotatef(-90, 1, 0, 0);

    // Base ring
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
    drawCylinder(0.12, 0.1, 0.1, 8);

    // Pillar body
    glTranslatef(0.0f, 0.0f, 0.1f);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_pillar);
    drawCylinder(0.1, 0.1, 2.0, 8);

    // Top ring
    glTranslatef(0.0f, 0.0f, 2.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
    drawCylinder(0.1, 0.12, 0.1, 8);

    glPopMatrix();

    // Railings (Between pillars)
    // Draw a railing segment to the next pillar
    glPushMatrix();
    float nextAngle = (i + 1) * 60.0f * M_PI / 180.0f;
    float x1 = r * cos(angle);
    float z1 = r * sin(angle);
    float x2 = r * cos(nextAngle);
    float z2 = r * sin(nextAngle);

    // Midpoint
    float mx = (x1 + x2) / 2.0f;
    float mz = (z1 + z2) / 2.0f;
    float dist = sqrt(pow(x2 - x1, 2) + pow(z2 - z1, 2));
    float rot = atan2(z2 - z1, x2 - x1) * 180.0f / M_PI;

    glTranslatef(mx, 0.8f, mz); // Railing height
    glRotatef(-rot, 0, 1, 0);

    // Top Rail
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0.0f, 0.0f, -dist / 2);
    drawCylinder(0.03, 0.03, dist, 4);
    glPopMatrix();

    // Bottom Rail
    glTranslatef(0.0f, -0.4f, 0.0f);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0.0f, 0.0f, -dist / 2);
    drawCylinder(0.03, 0.03, dist, 4);
    glPopMatrix();

    // Balusters (Vertical bars)
    int bars = 3;
    for (int b = 1; b <= bars; b++) {
      float t = (float)b / (bars + 1);
      float bx = (t - 0.5f) * dist;
      glPushMatrix();
      glTranslatef(bx, 0.2f, 0.0f);
      glRotatef(-90, 1, 0, 0);
      drawCylinder(0.02, 0.02, 0.4, 4);
      glPopMatrix();
    }

    glPopMatrix();
  }

  // 3. Roof (Scarlet Red, Curved)
  glPushMatrix();
  glTranslatef(0.0f, 2.5f, 0.0f);
  GLfloat mat_roof[] = {0.8f, 0.1f, 0.1f, 1.0f}; // Scarlet
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_roof);

  // Main roof cone (Curved approximation using stacked cones)
  // Layer 1 (Bottom)
  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  drawCylinder(1.8, 1.4, 0.3, 8);
  glTranslatef(0.0f, 0.0f, 0.3f);
  drawCylinder(1.4, 1.0, 0.3, 8);
  glTranslatef(0.0f, 0.0f, 0.3f);
  drawCylinder(1.0, 0.6, 0.3, 8);
  glTranslatef(0.0f, 0.0f, 0.3f);
  drawCylinder(0.6, 0.0, 0.5, 8);
  glPopMatrix();

  // Finial (Gold)
  glTranslatef(0.0f, 1.4f, 0.0f);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
  drawSphere(0.2, 8, 8);
  glTranslatef(0.0f, 0.2f, 0.0f);
  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  drawCylinder(0.02, 0.0, 0.5, 4);
  glPopMatrix();

  glPopMatrix();

  glEndList();
}

void generateTree() {
  glNewList(listTree, GL_COMPILE);

  // Trunk (Dark Wood)
  GLfloat mat_wood[] = {0.3f, 0.2f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_wood);
  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  drawCylinder(0.2, 0.1, 1.5, 6);
  glPopMatrix();

  // Foliage (Deep Green, Spherical clumps for a more "fantasy" look)
  GLfloat mat_leaves[] = {0.0f, 0.4f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_leaves);

  // Main clump
  glPushMatrix();
  glTranslatef(0.0f, 2.0f, 0.0f);
  drawSphere(1.0, 8, 8);
  glPopMatrix();

  // Side clumps
  for (int i = 0; i < 4; i++) {
    glPushMatrix();
    float angle = i * 90.0f * M_PI / 180.0f;
    glTranslatef(0.6f * cos(angle), 1.5f, 0.6f * sin(angle));
    drawSphere(0.7, 6, 6);
    glPopMatrix();
  }

  glEndList();
}

void generateBridge() {
  glNewList(listBridge, GL_COMPILE);

  // Vermilion Bridge (Japanese/Fantasy Style)
  GLfloat mat_red[] = {0.8f, 0.2f, 0.1f, 1.0f};
  GLfloat mat_gold[] = {0.8f, 0.6f, 0.1f, 1.0f};

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_red);

  int segments = 12;
  float width = 2.0f;
  float length = 5.0f;
  float height = 0.8f; // Higher arch

  // Arch Structure
  glBegin(GL_QUADS);
  for (int i = 0; i < segments; i++) {
    float t1 = (float)i / segments;
    float t2 = (float)(i + 1) / segments;

    float x1 = (t1 - 0.5f) * length;
    float y1 = 4 * height * t1 * (1.0f - t1);
    float x2 = (t2 - 0.5f) * length;
    float y2 = 4 * height * t2 * (1.0f - t2);

    // Floor
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x1, y1, -width / 2);
    glVertex3f(x2, y2, -width / 2);
    glVertex3f(x2, y2, width / 2);
    glVertex3f(x1, y1, width / 2);

    // Side Walls (Red)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(x1, y1 - 0.2f, -width / 2);
    glVertex3f(x2, y2 - 0.2f, -width / 2);
    glVertex3f(x2, y2 + 0.3f, -width / 2); // Railing height
    glVertex3f(x1, y1 + 0.3f, -width / 2);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x1, y1 - 0.2f, width / 2);
    glVertex3f(x2, y2 - 0.2f, width / 2);
    glVertex3f(x2, y2 + 0.3f, width / 2);
    glVertex3f(x1, y1 + 0.3f, width / 2);
  }
  glEnd();

  // Gold Railing Caps
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
  for (int i = 0; i <= segments; i++) {
    float t = (float)i / segments;
    float x = (t - 0.5f) * length;
    float y = 4 * height * t * (1.0f - t) + 0.3f;

    glPushMatrix();
    glTranslatef(x, y, -width / 2);
    drawSphere(0.1, 6, 6);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y, width / 2);
    drawSphere(0.1, 6, 6);
    glPopMatrix();
  }

  glEndList();
}

void generateLampPost() {
  glNewList(listLampPost, GL_COMPILE);

  // Post (Black Iron)
  GLfloat mat_iron[] = {0.1f, 0.1f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_iron);

  glPushMatrix();
  glRotatef(-90, 1, 0, 0);
  drawCylinder(0.1, 0.05, 2.0, 6);
  glPopMatrix();

  // Lamp Head (Gold Frame)
  glTranslatef(0.0f, 2.0f, 0.0f);
  GLfloat mat_gold[] = {0.8f, 0.7f, 0.2f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gold);
  glutSolidCube(0.4);

  // Light (Emissive Yellow)
  GLfloat mat_light[] = {1.0f, 1.0f, 0.8f, 1.0f};
  GLfloat mat_emissive[] = {0.8f, 0.8f, 0.5f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_light);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emissive);

  glutSolidSphere(0.15, 6, 6);

  // Reset Emission
  GLfloat no_emit[] = {0.0f, 0.0f, 0.0f, 1.0f};
  glMaterialfv(GL_FRONT, GL_EMISSION, no_emit);

  glEndList();
}

// --- Public API ---

void initDisplayLists() {
  if (listGazebo == 0) {
    listGazebo = glGenLists(4);
    listTree = listGazebo + 1;
    listBridge = listGazebo + 2;
    listLampPost = listGazebo + 3;

    generateGazebo();
    generateTree();
    generateBridge();
    generateLampPost();
  }
}

void drawHexGazebo() { glCallList(listGazebo); }

void drawRealisticTree() { glCallList(listTree); }

void drawArchedBridge() { glCallList(listBridge); }

void drawLampPost() { glCallList(listLampPost); }

void drawSkyDome(float radius) {
  // Disable lighting for the sky to have pure colors
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

      // Gradient: Horizon (Orange) -> Zenith (Dark Blue)
      // i=0 is bottom (horizon for a dome usually starts at 0 if we only draw
      // half, but here we draw full sphere) Let's assume we only see the top
      // half mostly, but drawing full is safer. Map y-coordinate to color.

      float t = (z0 + 1.0f) * 0.5f; // 0 to 1
      glColor3f(0.8f - t * 0.8f, 0.4f - t * 0.4f,
                0.2f + t * 0.2f); // Simple gradient

      glVertex3f(x * zr0 * radius, z0 * radius, y * zr0 * radius);

      t = (z1 + 1.0f) * 0.5f;
      glColor3f(0.8f - t * 0.8f, 0.4f - t * 0.4f, 0.2f + t * 0.2f);
      glVertex3f(x * zr1 * radius, z1 * radius, y * zr1 * radius);
    }
    glEnd();
  }

  glEnable(GL_LIGHTING);
}
