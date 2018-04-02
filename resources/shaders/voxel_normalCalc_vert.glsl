#version  330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec3 vPosition;

out vec3 fPosition_World;

void main()
{
    gl_Position = P * V * M * vec4(vPosition, 1.0); //Move position to camera space
    fPosition_World = (M * vec4(vPosition, 1.0f)).xyz;
}

