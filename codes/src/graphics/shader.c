#include "shader.h"
#include "../utils/file_utils.h"
#include <stdio.h>
#include <stdlib.h>

void checkCompileErrors(GLuint shader, const char *type) {
  GLint success;
  GLchar infoLog[1024];
  if (type[0] != 'P') {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      printf("SHADER_COMPILATION_ERROR of type: %s\n%s\n", type, infoLog);
      exit(1);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      printf("PROGRAM_LINKING_ERROR of type: %s\n%s\n", type, infoLog);
      exit(1);
    }
  }
}

GLuint Shader_Create(const char *vertPath, const char *fragPath) {
  char *vSrc = readFile(vertPath);
  char *fSrc = readFile(fragPath);
  if (!vSrc || !fSrc) {
    printf("Failed to read shaders: %s, %s\n", vertPath, fragPath);
    exit(1);
  }

  GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, (const GLchar **)&vSrc, NULL);
  glCompileShader(vShader);
  checkCompileErrors(vShader, "VERTEX");

  GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, (const GLchar **)&fSrc, NULL);
  glCompileShader(fShader);
  checkCompileErrors(fShader, "FRAGMENT");

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vShader);
  glAttachShader(prog, fShader);
  glLinkProgram(prog);
  checkCompileErrors(prog, "PROGRAM");

  free(vSrc);
  free(fSrc);
  glDeleteShader(vShader);
  glDeleteShader(fShader);
  return prog;
}

void Shader_Use(GLuint program) { glUseProgram(program); }

void Shader_SetInt(GLuint program, const char *name, int value) {
  glUniform1i(glGetUniformLocation(program, name), value);
}
void Shader_SetFloat(GLuint program, const char *name, float value) {
  glUniform1f(glGetUniformLocation(program, name), value);
}
void Shader_SetVec3(GLuint program, const char *name, float x, float y,
                    float z) {
  glUniform3f(glGetUniformLocation(program, name), x, y, z);
}

void Shader_SetVec4(GLuint program, const char *name, float x, float y, float z,
                    float w) {
  glUniform4f(glGetUniformLocation(program, name), x, y, z, w);
}

void Shader_SetMat4(GLuint program, const char *name, const float *value) {
  glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, value);
}
