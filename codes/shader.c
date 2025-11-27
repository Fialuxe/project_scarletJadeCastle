#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to read file content
char *readFile(const char *filePath) {
  FILE *f = fopen(filePath, "r");
  if (!f) {
    printf("Failed to open shader file: %s\n", filePath);
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = (char *)malloc(length + 1);
  if (buffer) {
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
  }
  fclose(f);
  return buffer;
}

// Compile a single shader
GLuint compileShader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    printf("Shader Compilation Failed:\n%s\n", infoLog);
    return 0;
  }
  return shader;
}

GLuint loadShader(const char *vertexPath, const char *fragmentPath) {
  char *vertexCode = readFile(vertexPath);
  char *fragmentCode = readFile(fragmentPath);

  if (!vertexCode || !fragmentCode) {
    if (vertexCode)
      free(vertexCode);
    if (fragmentCode)
      free(fragmentCode);
    return 0;
  }

  GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
  GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

  if (!vertex || !fragment) {
    free(vertexCode);
    free(fragmentCode);
    return 0;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    printf("Program Linking Failed:\n%s\n", infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  free(vertexCode);
  free(fragmentCode);

  return program;
}
