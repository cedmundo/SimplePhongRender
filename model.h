#pragma once
#include <stddef.h>
#include <xmath/transform.h>

#include "camera.h"
#include "core.h"
#include "shader.h"
#include "texture.h"

// A single vertex representing the attributes required by the shader
typedef struct {
  Vec3 pos;
  Vec3 nor;
  Vec2 uvs;
  Vec4 col;
} Vertex;

// Primitive reflects a single mesh instance of a model
typedef struct {
  unsigned short *indices;
  size_t indicesCount;
  Vertex *vertices;
  size_t verticesCount;
  unsigned vbo;
  unsigned ebo;
} Mesh;

// Standard phong-based material
typedef struct {
  Texture baseColorTex;
  Shader shader;
} Material;

// Model wraps a mesh with a material, a transform and its buffers.
typedef struct {
  Mesh *meshes;
  size_t meshesCount;
  unsigned vao;

  Material material;
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
void RenderModel(Model model, Camera camera);
