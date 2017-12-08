#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 geometryNormal[];
in vec3 geometryWorldPosition[];

out vec3 fragmentNormal;
out vec3 worldPosition;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vec4 normal;
    
    for(int i = 0; i < 3; i++) {
        fragmentNormal = geometryNormal[i];
        worldPosition = geometryWorldPosition[i];
        normal = vec4(geometryNormal[i], 0.0f);
        gl_Position = gl_in[i].gl_Position;
        //+ rand(normal.xy)*0.1f;
        EmitVertex();
    }
    EndPrimitive();
}
