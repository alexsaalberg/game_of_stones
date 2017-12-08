#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 geometryNormal[];
in vec3 geometryWorldPosition[];
in vec2 geometryTextureCoordinates[];

uniform sampler2D Texture0;

out vec3 fragmentNormal;
out vec3 worldPosition;
out vec2 textureCoordinates;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
//in mat4 geomP[]:
//in mat4 geomV[];
//in mat4 geomM[];
in vec4 geom_vertexPosition_modelSpace[];
in vec3 geom_normal_modelSpace[];

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vec4 normal;
    float heightScale = 0.05f;
    float height;
    vec4 position;
    
    for(int i = 0; i < 3; i++) {
        position = P * V * M * geom_vertexPosition_modelSpace[i];
        fragmentNormal = (V * M * vec4(geom_normal_modelSpace[i], 0.0)).xyz; //Don't want to move normal into perspective
        
        //fragmentNormal = geometryNormal[i];
        //worldPosition = geometryWorldPosition[i];
        textureCoordinates = geometryTextureCoordinates[i];
        vec4 textureValue = texture(Texture0, textureCoordinates);
        
        normal = vec4(fragmentNormal, 0.0f);
        
        height = heightScale * (textureValue.r*4 + textureValue.g*2 - textureValue.b*3);
        
        height = -height; //inward
        
        vec4 heightVector = normal * height;
        
        gl_Position = position + heightVector;
        worldPosition = vec3(V * M * gl_Position);
        //+ rand(normal.xy)*0.1f;
        EmitVertex();
    }
    EndPrimitive();
}
