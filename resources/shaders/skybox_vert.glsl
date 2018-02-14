#version 330 core

layout(location = 0) in vec3 vp;
uniform mat4 P, V;
out vec3 texcoords;

void main() {
  texcoords = vp;
  gl_Position = P * V * vec4(vp, 1.0);
}