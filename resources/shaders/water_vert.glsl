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
const float amplitude = 0.2f;
const float phase = 2.2f;
const float phi = 1.2f;
vec3 wavePosition = vec3(20,0,10);
void main()
{
    
    vec2 texcoord = vertTex + offset;
    vec4 tpos = vertPos;
    vec4 worldPos = P * V * M * tpos;
    //TODO: finetuning these sin/cos Amplitude values. Actually, look into gerstner waves.
    //tpos.y += 1*sin(tpos.x*1.2*w/25) + 1.2*cos(tpos.y*.5*w/25);
    vec2 direction = (wavePosition - tpos.xyz).xz;
    //tpos.y = .1*sin(5*tpos.x + w/10) + .1*cos(3*tpos.y + w/10);
    tpos.y += amplitude*sin(dot(direction, vec2(tpos.x, tpos.z))*phi + w*phase*15);
    //tpos.yz += offset;
    
    
    gl_Position = P * V * M * tpos;
    
    fragNor = (M * vec4(vertNor, 0.0)).xyz;
    fragTex = texcoord;
    
}