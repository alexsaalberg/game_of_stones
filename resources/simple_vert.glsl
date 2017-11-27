#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 MV;
out vec3 fragNor;
out vec3 WPos;

void main()
{
	gl_Position = P * MV * vertPos;
	fragNor = (MV * vec4(vertNor, 0.0)).xyz;
	WPos = vec3(MV*vertPos);
}
