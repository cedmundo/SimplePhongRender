#version 330 core

in vec4 vCol;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 materialBaseColor;
uniform float materialMetalness;
uniform float materialRoughness;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 ambientCol;

void main() {
  // diffuse
  vec3 normDir = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diffIntensity = max(dot(normDir, lightDir), 0.0);
  vec3 diffuse = diffIntensity * lightCol;

  // specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, normDir);
  float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), materialMetalness);
  vec3 specular = 1.0 * specIntensity * lightCol;

  // result
  FragColor = vec4((ambientCol + diffuse + specular) * materialBaseColor, 1.0);
}
