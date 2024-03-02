#pragma once
#include "core.h"

// Shader holds the program id after loading
typedef struct {
  unsigned spId;
  StatusCode status;
} Shader;

// Load, compile and link a shader program using a fragment and vertex shaders.
Shader LoadShader(const char *vsPath, const char *fsPath);

// Destroy a shader if needed.
void DestroyShader(Shader shader);
