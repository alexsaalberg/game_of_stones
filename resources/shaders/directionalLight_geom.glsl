#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 geometryNormal[];
in vec3 geometryWorldPosition[];

out vec3 fragmentNormal;
out vec3 worldPosition;

void main()
{
    
    for(int i = 0; i < 3; i++) {
        fragmentNormal = geometryNormal[i];
        worldPosition = geometryWorldPosition[i];
        vec4 normal = vec4(normalize(geometryNormal[i]), 1.0f) ;
        gl_Position = gl_in[i].gl_Position; //+ normal*0.1f;
        EmitVertex();
    }
    
    EndPrimitive();
}
