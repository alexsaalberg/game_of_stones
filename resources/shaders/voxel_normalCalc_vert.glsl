#version  330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec3 vPosition;
layout(location = 2) in int block_type;

out vec3 fPosition_World;
out vec4 block_color;

void main()
{
    gl_Position = P * V * M * vec4(vPosition, 1.0); //Move position to camera space
    fPosition_World = (M * vec4(vPosition, 1.0f)).xyz;
    if(block_type == 1) {
        block_color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    } else if (block_type == 255-128) {
        block_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    } else {
        block_color = vec4(1.0f, 0.0, 0.0f, 0.5f);
    }
}

