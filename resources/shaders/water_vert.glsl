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
const float amplitude = 0.4f;
const float phase = 0.6f;
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
    //tpos.z += .1*sin(tpos.x + w/50) + .1*cos(tpos.y + w/10);
    tpos.z += .3*sin(.2*tpos.x + w/50) + .2*cos(.5*tpos.y + w/50);
    //tpos.z += amplitude*sin(dot(direction, vec2(tpos.x, tpos.z))*phi + w*phase*10);
    //tpos.yz += offset;
    
    
    gl_Position = P * V * M * tpos;
    
    //fragNor = (M * vec4(vertNor, 0.0)).xyz;
    fragTex = texcoord;
    
}
