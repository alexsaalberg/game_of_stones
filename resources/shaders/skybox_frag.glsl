#version 330 core

layout(location = 0) in vec3 texcoords;
uniform samplerCube cube_texture;
out vec4 frag_colour;

void main() {
  frag_colour = texture(cube_texture, texcoords);
}