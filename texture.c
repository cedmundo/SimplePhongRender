#include "texture.h"
#include <glad/glad.h>

Texture LoadTexture(const char *texPath) {
  // TODO(cedmundo): verify textPath is a KTX texture
  Texture texture = {0};
  KTX_error_code result = KTX_SUCCESS;

  result = ktxTexture_CreateFromNamedFile(
      texPath, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture.src);
  if (result != KTX_SUCCESS) {
    // TODO(cedmundo): Handle KTX_* errors
    // TODO(cedmundo): Report log error
    texture.status = E_CANNOT_CREATE_TEXTURE;
    return texture;
  }

  // upload to GPU
  GLenum glError = 0;
  glGenTextures(1, &texture.id);
  result =
      ktxTexture_GLUpload(texture.src, &texture.id, &texture.format, &glError);
  if (result != KTX_SUCCESS) {
    // TODO(cedmundo): Handle KTX_* errors and glError
    // TODO(cedmundo): Report log error
    texture.status = E_CANNOT_UPLOAD_TEXTURE;
    return texture;
  }

  return texture;
}

void DestroyTexture(Texture texture) {
  if (texture.id != 0) {
    glDeleteTextures(1, &texture.id);
  }

  if (texture.src != NULL) {
    ktxTexture_Destroy(texture.src);
  }
}
