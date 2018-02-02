#version  410 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 offset;
uniform float w;

out vec3 fragNor;
out vec2 fragTex;


uniform sampler2D tex;

void main()
{
    vec2 texcoord = vertTex + offset;
    vec4 tpos = vertPos;
    //TODO: finetuning these sin/cos Amplitude values. Actually, look into gerstner waves.
    //tpos.y += 1*sin(tpos.x*1.2*w/25) + 1.2*cos(tpos.y*.5*w/25);
    //tpos.y -= .15*sin(w*(sqrt(tpos.x*tpos.x+ tpos.z*tpos.z) * 3.14));
    //tpos.yz += offset;
    
    
    gl_Position = P * V * M * tpos;
    fragNor = (M * vec4(vertNor, 0.0)).xyz;
    fragTex = texcoord;
    
}