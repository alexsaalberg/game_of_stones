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

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vec4 normal;
    
    for(int i = 0; i < 3; i++) {
        fragmentNormal = geometryNormal[i];
        worldPosition = geometryWorldPosition[i];
        textureCoordinates = geometryTextureCoordinates[i];
        vec4 textureValue = texture(Texture0, textureCoordinates);
        
        normal = vec4(geometryNormal[i], 0.0f);
        gl_Position = gl_in[i].gl_Position + normal * textureValue * 0.2f;
        //+ rand(normal.xy)*0.1f;
        EmitVertex();
    }
    EndPrimitive();
}
