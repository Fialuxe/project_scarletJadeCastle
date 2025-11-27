#include "camera.h"
#include <math.h>

void Camera_UpdateVectors(Camera *cam) {
  vec3 front;
  front.x = cosf(cam->Yaw * M_PI / 180.0f) * cosf(cam->Pitch * M_PI / 180.0f);
  front.y = sinf(cam->Pitch * M_PI / 180.0f);
  front.z = sinf(cam->Yaw * M_PI / 180.0f) * cosf(cam->Pitch * M_PI / 180.0f);
  cam->Front = normalize(front);
  cam->Right = normalize(cross(cam->Front, cam->WorldUp));
  cam->Up = normalize(cross(cam->Right, cam->Front));
}

void Camera_Init(Camera *cam, vec3 position, vec3 up, float yaw, float pitch) {
  cam->Position = position;
  cam->WorldUp = up;
  cam->Yaw = yaw;
  cam->Pitch = pitch;
  cam->Front = (vec3){0.0f, 0.0f, -1.0f};
  cam->MovementSpeed = 2.5f;
  cam->MouseSensitivity = 0.1f;
  Camera_UpdateVectors(cam);
}

mat4 Camera_GetViewMatrix(Camera *cam) {
  return lookAt(cam->Position, vecAdd(cam->Position, cam->Front), cam->Up);
}

void Camera_ProcessKeyboard(Camera *cam, CameraMovement direction,
                            float deltaTime) {
  float velocity = cam->MovementSpeed * deltaTime;
  if (direction == CAM_FORWARD)
    cam->Position = vecAdd(cam->Position, vecMul(cam->Front, velocity));
  if (direction == CAM_BACKWARD)
    cam->Position = vecSub(cam->Position, vecMul(cam->Front, velocity));
  if (direction == CAM_LEFT)
    cam->Position = vecSub(cam->Position, vecMul(cam->Right, velocity));
  if (direction == CAM_RIGHT)
    cam->Position = vecAdd(cam->Position, vecMul(cam->Right, velocity));
}

void Camera_ProcessMouseMovement(Camera *cam, float xoffset, float yoffset) {
  xoffset *= cam->MouseSensitivity;
  yoffset *= cam->MouseSensitivity;

  cam->Yaw += xoffset;
  cam->Pitch += yoffset;

  if (cam->Pitch > 89.0f)
    cam->Pitch = 89.0f;
  if (cam->Pitch < -89.0f)
    cam->Pitch = -89.0f;

  Camera_UpdateVectors(cam);
}
