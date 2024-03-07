#include "camera.h"
#include "core.h"
#include "model.h"
#include "xmath/transform.h"
#include "xmath/vec3.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGTH 500
#define WINDOW_TITLE "SimplePhongRender"

int main() {
  StatusCode status;

  status = AppInit(WINDOW_WIDTH, WINDOW_HEIGTH, WINDOW_TITLE);
  if (status != SUCCESS) {
    return AppClose(status);
  }

  Shader shader = LoadShader("assets/phong.vs.glsl", "assets/phong.fs.glsl");
  if (shader.status != SUCCESS) {
    return AppClose(shader.status);
  }

  Model model = LoadModel("assets/cube.gltf");
  if (model.status != SUCCESS) {
    return AppClose(model.status);
  }
  model.transform = MakeTransform();

  Camera camera = MakeDefaultCamera(Vec3Up);
  camera.position = (Vec3){0.0f, 1.0f, 10.0f};

  LightSource ambientLight = {LIGHT_AMBIENT, ColorDarkGray, MakeTransform()};
  LightSource spotlight = {LIGHT_POINTLIGHT, ColorWhite, MakeTransform()};
  spotlight.transform.origin = (Vec3){0.0, 0.0, 10.0};

  while (!AppShouldClose()) {
    BeginFrame();
    {
      // Update camera and look at cube
      UpdateCamera(&camera);

      // camera.position =
      //    Vec3Add(camera.position, (Vec3){0.0f, GetDeltaTime() * 0.2f, 0});

      // Rotate cube
      model.transform.angles.y += 1.0f * GetDeltaTime();
      // model.transform.angles.x += 1.0f * GetDeltaTime();

      // Render
      RenderModel(model, camera, spotlight, ambientLight);
    }
    EndFrame();
  }

  DestroyShader(shader);
  DestroyModel(model);
  return AppClose(SUCCESS);
}
