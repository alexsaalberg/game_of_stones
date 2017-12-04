#version 330 core

// Input vertex data, different for all executions of this shader.
// layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 0) in vec3 vertPos;

// Values that stay constant for the whole mesh.
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
    mat4 depthMVP = P * V * M;
//gl_Position =  depthMVP * vec4(vertexPosition_modelspace,1);
    gl_Position =  depthMVP * vec4(vertPos,1);
}

