#version 330 core
uniform sampler2D Texture0;

in vec3 fragNor;
in vec2 fragTex;
in float dCo;
out vec4 color;

void main() {
    //vec4 texColor0 = texture(Texture0, vTexCoord);
    
    //Outcolor = dCo*texColor0;
    color = texture(Texture0, fragTex);
    //float dist = length(pos);
    //float s = dist/50.0;
    //s = pow(s, 5)-0.2;
    //color.a = s;
    
    
    //DEBUG:Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}

