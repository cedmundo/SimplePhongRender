#include "core.h"
#include "model.h"

#include <glad/glad.h>
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

Model MakeCube(float dim) {
  Model model = {0};
  model.meshesCount = 1;
  model.meshes = calloc(1, sizeof(Mesh));
  Mesh *mesh = model.meshes;

  // a plane
  // clang-format off
  float vertices[] = {
    // POSITIONS
    -dim, -dim,  dim, //0
     dim, -dim,  dim, //1
    -dim,  dim,  dim, //2
     dim,  dim,  dim, //3
    -dim, -dim, -dim, //4
     dim, -dim, -dim, //5
    -dim,  dim, -dim, //6
     dim,  dim, -dim  //7
  };
  unsigned short indices[] = {
    //Top
    2, 6, 7,
    2, 3, 7,

    //Bottom
    0, 4, 5,
    0, 1, 5,

    //Left
    0, 2, 6,
    0, 4, 6,

    //Right
    1, 3, 7,
    1, 5, 7,

    //Front
    0, 2, 3,
    0, 1, 3,

    //Back
    4, 6, 7,
    4, 5, 7
  };
  // clang-format on
  mesh->indicesCount = 36;

  // upload model
  glGenVertexArrays(1, &model.vao);
  glGenBuffers(1, &mesh->vbo);
  glGenBuffers(1, &mesh->ebo);

  glBindVertexArray(model.vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return model;
}

Model LoadModel(const char *path) {
  Model model = LoadGLTF(path);
  if (model.status != SUCCESS) {
    return model;
  }

  model.status = UploadModel(&model);
  if (model.status != SUCCESS) {
    return model;
  }

  return model;
}

Model LoadGLTF(const char *path) {
  Model model = {0};

  cgltf_options options = {0};
  cgltf_data *data = NULL;
  cgltf_result result;

  // Open file, validate its contents and load external buffers if needed
  result = cgltf_parse_file(&options, path, &data);
  if (result != cgltf_result_success) {
    model.status = E_CANNOT_LOAD_FILE;
    Log(LOG_ERROR, "could not load model: %s, result: %d", path, result);
    return model;
  }

  result = cgltf_validate(data);
  if (result != cgltf_result_success) {
    model.status = E_CANNOT_LOAD_FILE;
    Log(LOG_ERROR, "invalid model: %s, result: %d", path, result);
    return model;
  }

  result = cgltf_load_buffers(&options, data, path);
  if (result != cgltf_result_success) {
    model.status = E_CANNOT_LOAD_FILE;
    Log(LOG_ERROR, "error loading buffers of file: %s, result: %d", path,
        result);
    return model;
  }

  size_t meshesCount = 0;
  for (size_t i = 0; i < data->meshes_count; i++) {
    meshesCount += data->meshes[i].primitives_count;
  }

  Mesh *meshes = calloc(meshesCount, sizeof(Mesh));
  if (meshes == NULL) {
    model.status = E_OUT_OF_MEMORY;
    Log(LOG_ERROR, "error loading file: %s (out of memory), result: %d", path,
        result);
    return model;
  }

  size_t meshIndex = 0;
  for (size_t mi = 0; mi < data->meshes_count; mi++) {
    for (size_t pi = 0; pi < data->meshes[mi].primitives_count; pi++) {
      Mesh *mesh = meshes + meshIndex;
      cgltf_primitive primitive = data->meshes[mi].primitives[pi];

      char *pos_buffer = NULL;
      size_t pos_stride = 0;

      char *nor_buffer = NULL;
      size_t nor_stride = 0;

      char *uvs_buffer = NULL;
      size_t uvs_stride = 0;

      char *col_buffer = NULL;
      size_t col_stride = 0;

      // Load model attributes (position, normals, color, uvs, etc)
      for (size_t ai = 0; ai < primitive.attributes_count; ai++) {
        cgltf_attribute attribute = primitive.attributes[ai];
        cgltf_accessor *attr_accessor = attribute.data;
        cgltf_buffer_view *attr_view = attr_accessor->buffer_view;
        cgltf_buffer *attr_buf = attr_view->buffer;

        if (attribute.type == cgltf_attribute_type_position) {
          if (attr_accessor->component_type != cgltf_component_type_r_32f ||
              attr_accessor->type != cgltf_type_vec3) {
            Log(LOG_WARN,
                "error loading pos attribute #%d, type %d in file %s (not "
                "a vec3 of floats)",
                ai, attribute.type, path);
            model.status = E_CANNOT_LOAD_FILE;
            return model;
          }

          pos_buffer = attr_buf->data + attr_view->offset;
          pos_stride = attr_accessor->stride;
          // Important: positions are the same as vertex count, so do not
          // remove from here.
          mesh->verticesCount = attr_accessor->count;
        } else if (attribute.type == cgltf_attribute_type_color) {
          cgltf_accessor *attr_accessor = attribute.data;
          if (attr_accessor->component_type != cgltf_component_type_r_32f ||
              attr_accessor->type != cgltf_type_vec4) {
            Log(LOG_WARN,
                "error loading color attribute #%d, type %d in file %s (not "
                "a vec4 of floats)",
                ai, attribute.type, path);
            // model.status = E_CANNOT_LOAD_FILE;
            // return model;
            continue;
          }

          col_buffer = attr_buf->data + attr_view->offset;
          col_stride = attr_accessor->stride;
        } else if (attribute.type == cgltf_attribute_type_texcoord) {
          cgltf_accessor *attr_accessor = attribute.data;
          if (attr_accessor->component_type != cgltf_component_type_r_32f ||
              attr_accessor->type != cgltf_type_vec2) {
            Log(LOG_WARN,
                "error loading texcoord attribute #%d, type %d in file %s (not "
                "a vec2 of floats)",
                ai, attribute.type, path);
            // model.status = E_CANNOT_LOAD_FILE;
            // return model;
            continue;
          }

          uvs_buffer = attr_buf->data + attr_view->offset;
          uvs_stride = attr_accessor->stride;
        } else if (attribute.type == cgltf_attribute_type_normal) {
          cgltf_accessor *attr_accessor = attribute.data;
          if (attr_accessor->component_type != cgltf_component_type_r_32f ||
              attr_accessor->type != cgltf_type_vec3) {
            Log(LOG_WARN,
                "error loading normal attribute #%d, type %d in file %s (not a "
                "vec3 of floats)",
                ai, attribute.type, path);
            // model.status = E_CANNOT_LOAD_FILE;
            // return model;
            continue;
          }

          nor_buffer = attr_buf->data + attr_view->offset;
          nor_stride = attr_accessor->stride;
        } else {
          Log(LOG_WARN,
              "ignoring attribute #%d ,type %d in file %s (not supported)", ai,
              attribute.type, path);
          continue;
        }
      }

      if (pos_buffer == NULL) {
        Log(LOG_ERROR, "There is no positions for the vertices");
        return model;
      }

      // Allocate space for all vertices in the mesh
      mesh->vertices = calloc(mesh->verticesCount, sizeof(Vertex));
      if (mesh->vertices == NULL) {
        Log(LOG_ERROR, "out of memory error: cannot load vertices of file %s",
            path);
        model.status = E_OUT_OF_MEMORY;
        return model;
      }

      for (size_t vi = 0; vi < mesh->verticesCount; vi++) {
        mesh->vertices[vi].pos = *((Vec3 *)(pos_buffer + pos_stride * vi));

        if (nor_buffer != NULL) {
          mesh->vertices[vi].nor = *((Vec3 *)(nor_buffer + nor_stride * vi));
        }

        if (uvs_buffer != NULL) {
          mesh->vertices[vi].uvs = *((Vec2 *)(uvs_buffer + uvs_stride * vi));
        }

        if (col_buffer != NULL) {
          mesh->vertices[vi].col = *((Vec4 *)(col_buffer + col_stride * vi));
        }
      }

      // Load model indices
      cgltf_accessor *indices_accessor = primitive.indices;
      if (indices_accessor->type != cgltf_type_scalar &&
          indices_accessor->component_type == cgltf_component_type_r_16u) {
        Log(LOG_ERROR,
            "indices accessor must be unsigned short scalar in file %s", path);
        model.status = E_CANNOT_LOAD_FILE;
        return model;
      }

      cgltf_buffer_view *indices_view = indices_accessor->buffer_view;
      cgltf_buffer *indices_buffer = indices_view->buffer;

      mesh->indicesCount = indices_accessor->count;
      mesh->indices = calloc(mesh->indicesCount, sizeof(unsigned short));
      if (mesh->indices == NULL) {
        Log(LOG_ERROR, "out of memory error: cannot load indices of file %s",
            path);
        model.status = E_OUT_OF_MEMORY;
        return model;
      }
      memcpy(mesh->indices, indices_buffer->data + indices_view->offset,
             indices_view->size);

      // Load materials
      cgltf_material *material = primitive.material;
      if (material->has_pbr_metallic_roughness) {
        // TODO(cedmundo): Load textures
        cgltf_pbr_metallic_roughness params = material->pbr_metallic_roughness;
        Material material = {0};
        Shader shader =
            LoadShader(MATERIAL_PHONG_VS_SHADER, MATERIAL_PHONG_FS_SHADER);
        if (shader.status != SUCCESS) {
          Log(LOG_ERROR,
              "cannot load default phong shader for PBR material in file %s",
              path);
          model.status = shader.status;
          return model;
        }

        material.baseColorFactor = (Color){
            params.base_color_factor[0],
            params.base_color_factor[1],
            params.base_color_factor[2],
            params.base_color_factor[3],
        };
        material.metallicFactor = params.metallic_factor;
        material.roughnessFactor = params.roughness_factor;
        material.shader = shader;
        mesh->material = material;
      }

      // Next mesh
      meshIndex++;
    }
  }

  // Collect evereything and return it so we can release it when destroying
  model.meshesCount = meshesCount;
  model.meshes = meshes;

  glBindVertexArray(0);
  cgltf_free(data);
  return model;
}

StatusCode UploadModel(Model *model) {
  glGenVertexArrays(1, &model->vao);
  glBindVertexArray(model->vao);

  for (size_t i = 0; i < model->meshesCount; i++) {
    Mesh *mesh = model->meshes + i;
    // Upload buffer and attributes
    glGenBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(mesh->verticesCount * sizeof(Vertex)),
                 mesh->vertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, pos));

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, nor));

    // TexCoord attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, uvs));

    // Color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, col));

    // Upload indices
    glGenBuffers(1, &mesh->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)mesh->indicesCount * sizeof(unsigned short),
                 mesh->indices, GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
  return SUCCESS;
}

void DestroyModel(Model model) {
  if (model.meshes != NULL) {
    for (int i = 0; i < model.meshesCount; i++) {
      DestroyMesh(model.meshes[i]);
    }

    free(model.meshes);
  }

  if (model.vao != 0) {
    glDeleteVertexArrays(1, &model.vao);
  }
}

void DestroyMesh(Mesh mesh) {
  if (mesh.indices != NULL) {
    free(mesh.indices);
  }

  if (mesh.vertices != NULL) {
    free(mesh.vertices);
  }

  if (mesh.ebo != 0) {
    glDeleteBuffers(1, &mesh.ebo);
  }

  if (mesh.vbo != 0) {
    glDeleteBuffers(1, &mesh.vbo);
  }
}

void RenderModel(Model model, Camera camera, LightSource mainLight,
                 LightSource ambientLight) {
  // Draw meshes
  glBindVertexArray(model.vao);
  for (int i = 0; i < model.meshesCount; i++) {
    Mesh mesh = model.meshes[i];
    Material material = mesh.material;
    Shader shader = material.shader;

    // TODO(cedmundo): Replace for MaterialUse(model.material)
    ShaderUse(mesh.material.shader);

    // Setup uniforms
    Mat4 viewMat = CameraGetViewMatrix(camera);
    Mat4 projMat = CameraGetProjMatrix(camera);
    Mat4 modelMat = TransformGetModelMatrix(model.transform);

    // position and transform
    ShaderSetUniformMat4(shader, "view", viewMat);
    ShaderSetUniformMat4(shader, "proj", projMat);
    ShaderSetUniformMat4(shader, "model", modelMat);

    // material
    ShaderSetUniformVec3(shader, "materialBaseColor",
                         ColorToRGB(material.baseColorFactor));
    ShaderSetUniformFloat(shader, "materialMetalness", material.metallicFactor);
    ShaderSetUniformFloat(shader, "materialRoughness",
                          material.roughnessFactor);

    // FIXME(cedmundo): camera position is not right, it camera.transform.origin
    // does not correspond to the actual position of the camera.
    ShaderSetUniformVec3(shader, "viewPos", camera.position);
    ShaderSetUniformVec3(shader, "lightPos", mainLight.transform.origin);
    ShaderSetUniformVec3(shader, "lightCol", ColorToRGB(mainLight.color));
    ShaderSetUniformVec3(shader, "ambientCol", ColorToRGB(ambientLight.color));

    // IMPORTANT NOTE: Maybe assign a type GL_UNSIGNED_SHORT | GL_UNSIGNED_INT
    // in case of getting a larger type at reading model.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indicesCount, GL_UNSIGNED_SHORT,
                   0);
  }

  glBindVertexArray(0);
}
