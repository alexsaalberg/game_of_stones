#version 330 core
layout(location = 0) in vec2 vPosition;

uniform mat4 P;
uniform mat4 V;

void main()
{
    gl_Position = P * V * vec4(vPosition, 0.0,  1.0); 
}
