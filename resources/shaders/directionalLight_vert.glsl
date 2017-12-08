#version  330 core
layout(location = 0) in vec4 vertexPosition_modelSpace;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTextureCoordinates;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 fragmentNormal;
out vec3 worldPosition;

void main()
{
    gl_Position = P * V * M * vertexPosition_modelSpace;
    fragmentNormal = (V * M * vec4(vertexNormal, 0.0)).xyz; //Don't want to move normal into perspective
    worldPosition = vec3(V * M * vertexPosition_modelSpace);
}

