#include "math_utils.h"
#include <math.h>

/*
commented out because of performance issue
vec3 normalize(vec3 v) {
  float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  if (len == 0)
    return v;
  vec3 r = {v.x / len, v.y / len, v.z / len};
  return r;
}*/

// Fast Inverse Square Root approximation (1 / sqrt(x))
// Based on the Quake III Arena algorithm
float Q_rsqrt(float number) {
  int i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y = number;

  // Evil floating point bit level hacking: interpret float bits as integer
  i = *(int *)&y;

  // Magic number to calculate initial guess
  i = 0x5f3759df - (i >> 1);

  // Convert integer back to float
  y = *(float *)&i;

  // 1st iteration of Newton's method for higher precision
  y = y * (threehalfs - (x2 * y * y));

  return y;
}

// Normalize vector using fast inverse square root
vec3 normalize(vec3 v) {
  // Calculate squared length (magnitude squared)
  float sqLen = v.x * v.x + v.y * v.y + v.z * v.z;

  // Prevent division by zero (or very small values)
  if (sqLen < 1e-6f)
    return v;

  // Calculate 1 / sqrt(sqLen) efficiently
  float invLen = Q_rsqrt(sqLen);

  // Multiply by the inverse length instead of dividing
  vec3 r = {v.x * invLen, v.y * invLen, v.z * invLen};
  return r;
}

mat4 identity() {
  mat4 res = {0};
  res.m[0] = res.m[5] = res.m[10] = res.m[15] = 1.0f;
  return res;
}

mat4 perspective(float fov, float aspect, float near, float far) {
  mat4 res = {0};
  float tanHalfFov = tanf(fov / 2.0f);
  res.m[0] = 1.0f / (aspect * tanHalfFov);
  res.m[5] = 1.0f / tanHalfFov;
  res.m[10] = -(far + near) / (far - near);
  res.m[11] = -1.0f;
  res.m[14] = -(2.0f * far * near) / (far - near);
  return res;
}

mat4 lookAt(vec3 eye, vec3 center, vec3 up) {
  vec3 f = vecSub(center, eye);
  f = normalize(f);

  vec3 s = cross(f, up);
  s = normalize(s);

  vec3 u = cross(s, f);

  mat4 res = identity();
  res.m[0] = s.x;
  res.m[4] = s.y;
  res.m[8] = s.z;
  res.m[1] = u.x;
  res.m[5] = u.y;
  res.m[9] = u.z;
  res.m[2] = -f.x;
  res.m[6] = -f.y;
  res.m[10] = -f.z;
  res.m[12] = -dot(s, eye);
  res.m[13] = -dot(u, eye);
  res.m[14] = dot(f, eye);
  return res;
}

mat4 mat4_multiply(mat4 a, mat4 b) {
  mat4 res;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      res.m[i * 4 + j] =
          a.m[i * 4 + 0] * b.m[0 * 4 + j] + a.m[i * 4 + 1] * b.m[1 * 4 + j] +
          a.m[i * 4 + 2] * b.m[2 * 4 + j] + a.m[i * 4 + 3] * b.m[3 * 4 + j];
    }
  }
  return res;
}

mat4 translate(float x, float y, float z) {
  mat4 res = identity();
  res.m[12] = x;
  res.m[13] = y;
  res.m[14] = z;
  return res;
}

mat4 scale(float x, float y, float z) {
  mat4 res = identity();
  res.m[0] = x;
  res.m[5] = y;
  res.m[10] = z;
  return res;
}

mat4 rotate_y(float angle) {
  mat4 res = identity();
  float rad = angle * M_PI / 180.0f;
  float c = cosf(rad);
  float s = sinf(rad);
  res.m[0] = c;
  res.m[2] = -s;
  res.m[8] = s;
  res.m[10] = c;
  return res;
}

mat4 rotate_x(float angle) {
  mat4 res = identity();
  float rad = angle * M_PI / 180.0f;
  float c = cosf(rad);
  float s = sinf(rad);
  res.m[5] = c;
  res.m[6] = -s;
  res.m[9] = s;
  res.m[10] = c;
  return res;
}

mat4 rotate_z(float angle) {
  mat4 res = identity();
  float rad = angle * M_PI / 180.0f;
  float c = cosf(rad);
  float s = sinf(rad);
  res.m[4] = c;
  res.m[6] = -s;
  res.m[7] = s;
  res.m[5] = c;
  return res;
}
