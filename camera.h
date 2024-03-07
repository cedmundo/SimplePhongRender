#pragma once
#include <xmath/mat4.h>
#include <xmath/vec3.h>

#define CAMERA_DEFAULT_NEAR 0.1f
#define CAMERA_DEFAULT_FAR 100.0f
#define CAMERA_DEFAULT_FOV 45.0f

// Camera represents an usable view for the render
typedef struct {
  enum {
    CAMERA_MODE_ORTHO_PROJ,
    CAMERA_MODE_PERSPECTIVE_PROJ,
  } mode;
  Vec3 forward;
  Vec3 up;
  Vec3 right;
  Vec3 worldUp;
  Vec3 position;

  float fov;
  float width;
  float height;
  float aspect;
  float near;
  float far;
} Camera;

// Return a perspective camera looking at origin offset a little bit
Camera MakeDefaultCamera(Vec3 worldUp);

// Return the *updated* projection matrix of a camera.
Mat4 CameraGetProjMatrix(Camera camera);

// Return the *updated* view matrix for a camera.
Mat4 CameraGetViewMatrix(Camera camera);

// Updates the current camera using core state
void UpdateCamera(Camera *camera);
