#version 330 core

in vec2 vUvs;
in vec4 vCol;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 materialBaseColor;
uniform float materialMetalness;
uniform float materialRoughness;

uniform sampler2D tex0;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 ambientCol;

void main() {
  // diffuse
  vec3 normDir = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diffIntensity = max(dot(normDir, lightDir), 0.0);
  vec3 diffuse = 0.2 * diffIntensity * lightCol;

  // specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, normDir);
  float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), materialMetalness);
  vec3 specular = 0.2 * specIntensity * lightCol;

  // adding evereything
	vec3 fColor = (ambientCol + diffuse + specular + vCol.xyz) * materialBaseColor;

  // result
  FragColor = texture(tex0, vUvs) * vec4(fColor, 1.0);
}
