#include "camera.h"

#include "core.h"

Mat4 CameraGetProjMatrix(Camera camera) {
  assert((camera.mode == CAMERA_MODE_PERSPECTIVE_PROJ ||
          camera.mode == CAMERA_MODE_ORTHO_PROJ) &&
         "invalid state: unknown camera mode");
  if (camera.mode == CAMERA_MODE_PERSPECTIVE_PROJ) {
    return Mat4MakePerspective(camera.fov, camera.aspect, camera.near,
                               camera.far);
  }

  // FIXME(cedmundo): we might need to use -camera.width/2 and camera.width/2
  return Mat4MakeOrtho(0, camera.width, 0, camera.height, camera.near,
                       camera.far);
}

Mat4 CameraGetViewMatrix(Camera camera) {
  Vec3 r = camera.right;
  Vec3 u = camera.up;
  Vec3 f = camera.forward;
  Vec3 t = camera.position;
  // clang-format off
	return (Mat4) {
  	 r.x,  u.x,  f.x, 0.0f,
  	 r.y,  u.y,  f.y, 0.0f,
  	 r.z,  u.z,  f.z, 0.0f,
  	-t.x, -t.y, -t.z, 1.0f,
	};
  // clang-format on
}

Camera MakeDefaultCamera(Vec3 worldUp) {
  Camera camera = {0};
  camera.mode = CAMERA_MODE_PERSPECTIVE_PROJ;
  camera.forward = Vec3Forward;
  camera.up = Vec3Up;
  camera.worldUp = worldUp;
  camera.fov = CAMERA_DEFAULT_FOV;
  camera.near = CAMERA_DEFAULT_NEAR;
  camera.far = CAMERA_DEFAULT_FAR;
  camera.position = Vec3Zero;
  return camera;
}

void UpdateCamera(Camera *camera) {
  Vec2 viewportSize = GetViewportSize();
  camera->aspect = viewportSize.x / viewportSize.y;
  camera->width = viewportSize.x;
  camera->height = viewportSize.y;
  camera->right = Vec3Cross(camera->worldUp, camera->forward);
  camera->up = Vec3Cross(camera->forward, camera->right);
}
