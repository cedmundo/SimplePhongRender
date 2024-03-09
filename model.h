#pragma once
#include <stddef.h>
#include <xmath/color.h>
#include <xmath/transform.h>

#include "camera.h"
#include "core.h"
#include "shader.h"
#include "texture.h"

// TODO(cedmundo): move this into App-like configuration
#define MATERIAL_PHONG_VS_SHADER "assets/phong.vs.glsl"
#define MATERIAL_PHONG_FS_SHADER "assets/phong.fs.glsl"

typedef struct {
  enum {
    LIGHT_AMBIENT,
    LIGHT_POINTLIGHT,
  } mode;
  Color color;
  Transform transform;
} LightSource;

// A single vertex representing the attributes required by the shader
typedef struct {
  Vec3 pos;
  Vec3 nor;
  Vec2 uvs;
  Vec4 col;
} Vertex;

// Standard phong-based material
typedef struct {
  Texture baseColorTexture;
  Color baseColorFactor;
  Texture metallicTexture;
  float metallicFactor;
  Texture roughnessTexture;
  float roughnessFactor;
  Shader shader;
} Material;

// Primitive reflects a single mesh instance of a model
typedef struct {
  unsigned short *indices;
  size_t indicesCount;
  Vertex *vertices;
  size_t verticesCount;
  unsigned vbo;
  unsigned ebo;

  Material material;
} Mesh;

// Model wraps a mesh with a material, a transform and its buffers.
typedef struct {
  Mesh *meshes;
  size_t meshesCount;
  unsigned vao;

  Transform transform;
  StatusCode status;
} Model;

// Make a single plane
Model MakeCube(float dim);

// Load a GLTF model into memory decoding its data and uploading to the GPU.
Model LoadModel(const char *path);

// Load GLTF data into a model without uploading to GPU.
Model LoadGLTF(const char *path);

// Upload model to the GPU.
StatusCode UploadModel(Model *model);

// Destroy all contents of a model.
void DestroyModel(Model model);

// Delete all related buffer arrays of mesh
void DestroyMesh(Mesh mesh);

// Render a model from the point of view of given camera
void RenderModel(Model model, Camera camera, LightSource mainLight,
                 LightSource ambientLight);
