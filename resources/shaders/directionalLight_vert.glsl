#version  330 core
layout(location = 0) in vec4 vertexPosition_modelSpace;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTextureCoordinates;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 geometryNormal;
out vec3 geometryWorldPosition;

void main()
{
    gl_Position = P * V * M * vertexPosition_modelSpace;
    vec3 fragmentNormal = (V * M * vec4(vertexNormal, 0.0)).xyz; //Don't want to move normal into perspective
    geometryNormal = fragmentNormal;
    geometryWorldPosition = vec3(V * M * vertexPosition_modelSpace);
}

