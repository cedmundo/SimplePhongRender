#pragma once
#include "core.h"
#include <stdbool.h>

// Shader holds the program id after loading
typedef struct {
  unsigned spId;
  StatusCode status;
} Shader;

// Load, compile and link a shader program using a fragment and vertex shaders.
Shader LoadShader(const char *vsPath, const char *fsPath);

// Set context to shader
void ShaderUse(Shader shader);

// Set an uniform by name using a mat4 value
void ShaderSetUniformMat4(Shader shader, const char *name, Mat4 value);

// Set an uniform by name using a vec4 value
void ShaderSetUniformVec4(Shader shader, const char *name, Vec4 value);

// Set an uniform by name using a vec3 value
void ShaderSetUniformVec3(Shader shader, const char *name, Vec3 value);

// Set an uniform by name using a vec2 value
void ShaderSetUniformVec2(Shader shader, const char *name, Vec2 value);

// Set an uniform by name using a float value
void ShaderSetUniformFloat(Shader shader, const char *name, float value);

// Set an uniform by name using a int value
void ShaderSetUniformInt(Shader shader, const char *name, int value);

// Set an uniform by name using a bool value
void ShaderSetUniformBool(Shader shader, const char *name, bool value);

// Destroy a shader if needed.
void DestroyShader(Shader shader);
