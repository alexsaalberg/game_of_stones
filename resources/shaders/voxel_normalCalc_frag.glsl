#version 330 core

in vec3 fPosition_World;

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

//uniform sampler2D Texture0;

uniform mat4 P;
uniform mat4 V;

/*LIGHTING*/
uniform vec3 eyePosition;
uniform vec3 directionTowardsLight;

uniform vec3 mDiffusionCoefficient;
uniform vec3 mAmbientCoefficient;
uniform vec3 mSpecularCoefficient;

uniform float mSpecularAlpha;

void main()
{
    vec3 fNormal_world = cross(dFdy(fPosition_World.xyz), dFdx(fPosition_World.xyz));
    fNormal_world = normalize(fNormal_world);
    
    vec4 fNormal_camera_nohomo = P * V * vec4(fNormal_world, 0.0f);
    vec3 fNormal_camera = fNormal_camera_nohomo.xyz / fNormal_camera_nohomo.w;
    fNormal_camera = normalize(fNormal_camera);
    
    vec3 directionTowardsEye = normalize(eyePosition - fPosition_World);
    
    vec3 L = normalize(directionTowardsLight);
    vec3 N = normalize(fNormal_camera);
    vec3 H = normalize(directionTowardsLight + directionTowardsEye);
    
    vec3 lightColor = vec3(0.0f);
    
    lightColor += mAmbientCoefficient;
    lightColor += mDiffusionCoefficient * clamp(dot(L, N), 0.0, 1.0);
    lightColor += mSpecularCoefficient * pow( clamp(dot(H, N), 0.0, 1.0), mSpecularAlpha);
    
    color = vec4(lightColor, 1.0);
    //color = (vec4(lightColor, 1.0) + vec4(abs(fNormal_world), 1.0f)) / 2.0f;
    //color = vec4(abs(fNormal_world) * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);
}
