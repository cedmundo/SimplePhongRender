#include "camera.h"
#include "core.h"
#include "model.h"

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

  Model model = MakeCube(1.0f);
  if (model.status != SUCCESS) {
    return AppClose(model.status);
  }

  model.transform = MakeTransform();
  model.shader = shader;

  Camera camera = MakeDefaultCamera();
  while (!AppShouldClose()) {
    BeginFrame();
    {
      // Update
      UpdateCamera(&camera);
      model.transform.angles.y += 1.0f * GetDeltaTime();
      model.transform.angles.x += 1.0f * GetDeltaTime();

      // Render
      RenderModel(model, camera);
    }
    EndFrame();
  }

  DestroyShader(shader);
  DestroyModel(model);
  return AppClose(SUCCESS);
}
