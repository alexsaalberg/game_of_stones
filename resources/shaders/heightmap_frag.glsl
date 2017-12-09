#version 330 core 
in vec3 fragmentNormal;
in vec3 worldPosition;
in vec2 textureCoordinates;
//to send the color to a frame buffer
layout(location = 0) out vec4 color;

uniform sampler2D Texture0;

uniform vec3 EyePosition;

uniform vec3 MaterialDiffusionCoefficient;
uniform vec3 MaterialAmbientCoefficient;
uniform vec3 MaterialSpecularCoefficient;
uniform float MaterialSpecularAlpha;


/* Very simple Diffuse shader with a directional light*/
void main()
{
    vec3 lightColor = vec3(0.0f);
    
    vec3 directionTowardsEye = EyePosition - worldPosition;
    
    vec3 lightDirection = normalize(vec3(1, 1, 1));
    vec3 normal = normalize(fragmentNormal);
    vec3 halfVector = normalize(lightDirection + directionTowardsEye);
    
    lightColor += MaterialAmbientCoefficient;
    lightColor += MaterialDiffusionCoefficient * max( dot( lightDirection, normal), 0 );
    lightColor += MaterialSpecularCoefficient
                * pow( max( dot( halfVector, normal), 0), MaterialSpecularAlpha);
    color = vec4(lightColor, 1.0);
    vec4 textureColor = texture(Texture0, textureCoordinates);
    color = 0.5 * (color+textureColor);
    color.a = 0.0f;
}
