#version 330 core 
in vec3 fragmentNormal;
in vec3 worldPos;
//to send the color to a frame buffer
layout(location = 0) out vec4 color;

uniform vec3 MaterialAmbientCoefficient;
uniform vec3 MaterialDiffusionCoefficient;

/* Very simple Diffuse shader with a directional light*/
void main()
{
	vec3 lightColor;
    vec3 lightDirection = vec3(1, 1, 1);
	vec3 normal = normalize(fragmentNormal);
    
    lightColor = vec3(0.0f);
    lightColor += MaterialDiffusionCoefficient * max( dot(normalize(lightDirection), normal), 0 );
    lightColor += MaterialAmbientCoefficient;
	color = vec4(lightColor, 1.0);
}
