#ifndef SHADER_H
#define SHADER_H

#include "../core/window.h" // For GL types

GLuint Shader_Create(const char *vertPath, const char *fragPath);
void Shader_Use(GLuint program);
void Shader_SetInt(GLuint program, const char *name, int value);
void Shader_SetFloat(GLuint program, const char *name, float value);
void Shader_SetVec3(GLuint program, const char *name, float x, float y,
                    float z);
void Shader_SetVec4(GLuint program, const char *name, float x, float y, float z,
                    float w);
void Shader_SetMat4(GLuint program, const char *name, const float *value);

#endif
