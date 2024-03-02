#pragma once
#include "core.h"
#include <ktx.h>

typedef struct {
  StatusCode status;
  unsigned id;
  ktxTexture *src;
  GLenum format;
} Texture;

// Load and decode an image as texture
Texture LoadTexture(const char *texPath);

// Release all resources linked to a texture
void DestroyTexture(Texture texture);
