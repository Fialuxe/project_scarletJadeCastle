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

// Vector Operations
vec3 vecAdd(vec3 a, vec3 b);
vec3 vecSub(vec3 a, vec3 b);
vec3 vecMul(vec3 a, float s);
vec3 cross(vec3 a, vec3 b);
vec3 normalize(vec3 v);
float dot(vec3 a, vec3 b);

// Matrix Operations
mat4 identity();
mat4 perspective(float fov, float aspect, float near, float far);
mat4 lookAt(vec3 eye, vec3 center, vec3 up);

#endif
