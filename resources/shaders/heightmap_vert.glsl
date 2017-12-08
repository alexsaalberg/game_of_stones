#version  330 core
layout(location = 0) in vec4 vertexPosition_modelSpace;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTextureCoordinates;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 geometryNormal;
out vec3 geometryWorldPosition;
out vec2 geometryTextureCoordinates;

out vec4 geom_vertexPosition_modelSpace;
out vec3 geom_normal_modelSpace;

//out mat4 geomP:
//out mat4 geomV;
//out mat4 geomM;

void main()
{
    //geomP = P;
    //geomV = V;
    //geomM = M;
    gl_Position = P * V * M * vertexPosition_modelSpace;
    geometryNormal = (V * M * vec4(vertexNormal, 0.0)).xyz; //Don't want to move normal into perspective
    geometryWorldPosition = vec3(V * M * vertexPosition_modelSpace);
    geom_vertexPosition_modelSpace = vertexPosition_modelSpace;
    geom_normal_modelSpace = vertexNormal;
    geometryTextureCoordinates = vertexTextureCoordinates;
}

