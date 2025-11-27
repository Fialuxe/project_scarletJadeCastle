#ifndef CAMERA_H
#define CAMERA_H

#include "../utils/math_utils.h"

typedef enum { CAM_FORWARD, CAM_BACKWARD, CAM_LEFT, CAM_RIGHT } CameraMovement;

typedef struct {
  vec3 Position;
  vec3 Front;
  vec3 Up;
  vec3 Right;
  vec3 WorldUp;
  float Yaw;
  float Pitch;
  float MovementSpeed;
  float MouseSensitivity;
} Camera;

void Camera_Init(Camera *cam, vec3 position, vec3 up, float yaw, float pitch);
mat4 Camera_GetViewMatrix(Camera *cam);
void Camera_ProcessKeyboard(Camera *cam, CameraMovement direction,
                            float deltaTime);
void Camera_ProcessMouseMovement(Camera *cam, float xoffset, float yoffset);

#endif
