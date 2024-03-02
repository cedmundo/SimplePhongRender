#include "shader.h"
#include <glad/glad.h>
#include <stdlib.h>

Shader LoadShader(const char *vsPath, const char *fsPath) {
  Shader shader = {0};
  int glStatus = 0;
  char *vsSource = NULL;
  char *fsSource = NULL;
  unsigned vsId = 0;
  unsigned fsId = 0;
  char shaderLog[512] = {0};

  vsSource = LoadFileContents(vsPath);
  if (vsSource == NULL) {
    Log(LOG_ERROR, "cannot load shader file: %s", vsPath);
    shader.status = E_CANNOT_LOAD_FILE;
    goto terminate;
  }

  fsSource = LoadFileContents(fsPath);
  if (fsSource == NULL) {
    Log(LOG_ERROR, "cannot load shader file: %s", fsPath);
    shader.status = E_CANNOT_LOAD_FILE;
    goto terminate;
  }

  glStatus = 0;
  vsId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vsId, 1, (const char **)&vsSource, NULL);
  glCompileShader(vsId);
  glGetShaderiv(vsId, GL_COMPILE_STATUS, &glStatus);
  if (!glStatus) {
    shader.status = E_SHADER_COMPILE_ERROR;
    glGetShaderInfoLog(vsId, 512, NULL, shaderLog);
    Log(LOG_ERROR, "cannot compile vertex shader: %s: %s", vsPath, shaderLog);
    goto terminate;
  }

  glStatus = 0;
  fsId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fsId, 1, (const char **)&fsSource, NULL);
  glCompileShader(fsId);
  glGetShaderiv(fsId, GL_COMPILE_STATUS, &glStatus);
  if (!glStatus) {
    shader.status = E_SHADER_COMPILE_ERROR;
    glGetShaderInfoLog(fsId, 512, NULL, shaderLog);
    Log(LOG_ERROR, "cannot compile fragment shader: %s: %s", fsPath, shaderLog);
    goto terminate;
  }

  shader.spId = glCreateProgram();
  glAttachShader(shader.spId, vsId);
  glAttachShader(shader.spId, fsId);
  glLinkProgram(shader.spId);
  glGetShaderiv(shader.spId, GL_LINK_STATUS, &glStatus);
  if (!glStatus) {
    shader.status = E_SHADER_LINK_ERROR;
    glGetShaderInfoLog(shader.spId, 512, NULL, shaderLog);
    Log(LOG_ERROR, "cannot link shader program: %s", shaderLog);
    goto terminate;
  }
  shader.status = SUCCESS;

terminate:
  if (vsId != 0) {
    glDetachShader(shader.spId, vsId);
    glDeleteShader(vsId);
  }

  if (fsId != 0) {
    glDetachShader(shader.spId, fsId);
    glDeleteShader(fsId);
  }

  if (shader.status != SUCCESS) {
    DestroyShader(shader);
  }

  if (vsSource != NULL) {
    free(vsSource);
  }

  if (fsSource != NULL) {
    free(fsSource);
  }

  return shader;
}

void ShaderUse(Shader shader) { glUseProgram(shader.spId); }

void ShaderSetUniformMat4(Shader shader, const char *name, Mat4 value) {
  glUniformMatrix4fv(glGetUniformLocation(shader.spId, name), 1, GL_FALSE,
                     Mat4Raw(&value));
}

void ShaderSetUniformVec4(Shader shader, const char *name, Vec4 value) {
  glUniform4f(glGetUniformLocation(shader.spId, name), value.x, value.y,
              value.z, value.w);
}

void ShaderSetUniformVec3(Shader shader, const char *name, Vec3 value) {
  glUniform3f(glGetUniformLocation(shader.spId, name), value.x, value.y,
              value.z);
}

void ShaderSetUniformVec2(Shader shader, const char *name, Vec2 value) {
  glUniform2f(glGetUniformLocation(shader.spId, name), value.x, value.y);
}

void ShaderSetUniformFloat(Shader shader, const char *name, float value) {
  glUniform1f(glGetUniformLocation(shader.spId, name), value);
}

void ShaderSetUniformInt(Shader shader, const char *name, int value) {
  glUniform1i(glGetUniformLocation(shader.spId, name), value);
}

void ShaderSetUniformBool(Shader shader, const char *name, bool value) {
  glUniform1i(glGetUniformLocation(shader.spId, name), value);
}

void DestroyShader(Shader shader) {
  if (shader.spId != 0) {
    glDeleteProgram(shader.spId);
  }
}
