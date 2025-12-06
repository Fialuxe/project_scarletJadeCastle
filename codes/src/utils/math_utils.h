#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
  float m[16];
} mat4;
typedef struct {
  float x, y, z;
} vec3;
typedef struct {
  float x, y, z, w;
} vec4;

// Vector Operations
// static for optimization
static inline vec3 vecAdd(vec3 a, vec3 b) {
  vec3 r = {a.x + b.x, a.y + b.y, a.z + b.z};
  return r;
}

static inline vec3 vecSub(vec3 a, vec3 b) {
  vec3 r = {a.x - b.x, a.y - b.y, a.z - b.z};
  return r;
}

static inline vec3 vecMul(vec3 a, float s) {
  vec3 r = {a.x * s, a.y * s, a.z * s};
  return r;
}

static inline vec3 cross(vec3 a, vec3 b) {
  vec3 r = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
  return r;
}

static inline float dot(vec3 a, vec3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Q_rsqrt(float number);
vec3 normalize(vec3 v);

// Matrix Operations
mat4 identity();
mat4 perspective(float fov, float aspect, float near, float far);
mat4 lookAt(vec3 eye, vec3 center, vec3 up);
mat4 mat4_multiply(mat4 a, mat4 b);
mat4 translate(float x, float y, float z);
mat4 scale(float x, float y, float z);
mat4 rotate_x(float angle);
mat4 rotate_y(float angle);

#endif
