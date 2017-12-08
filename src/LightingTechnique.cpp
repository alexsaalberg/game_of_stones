//
//  LightingTechnique.cpp
//  final471project
//
//  Created by Alex Saalberg on 12/7/17.
//

#include "LightingTechnique.hpp"

void LightingTechnique::createLightingTechnique(std::shared_ptr<Program> prog) {
    
    
    
    
    prog->addUniform("matSpecularIntensity");
    prog->addUniform("matSpecularPower");
    
    dirLightLocation.Color = prog->addUniform("directionalLight.Base.Color");
    dirLightLocation.AmbientIntensity = prog->addUniform("directionalLight.Base.AmbientIntensity");
    dirLightLocation.DiffuseIntensity = prog->addUniform("directionalLight.Base.DiffuseIntensity");;
    dirLightLocation.Direction = prog->addUniform("directionalLight.Base.Direction");
    
    numPointLightsLocation = prog->addUniform("numPointLights");
    numSpotLightsLocation = prog->addUniform("numSpotLights");
    
    char nameBuffer[128];
    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        snprintf(nameBuffer, sizeof(nameBuffer), "pointLights[%d].Base.Color", i);
        pointLightsLocation[i].Color = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "pointLights[%d].Base.AmbientIntensity", i);
        pointLightsLocation[i].AmbientIntensity = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "pointLights[%d].Base.DiffuseIntesity", i);
        pointLightsLocation[i].DiffuseIntensity = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "pointLights[%d].Position", i);
        pointLightsLocation[i].Position = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "pointLights[%d].Atten.Constant", i);
        pointLightsLocation[i].Atten.Constant = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "pointLights[%d].Atten.Linear", i);
        pointLightsLocation[i].Atten.Linear = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "pointLights[%d].Atten.Exp", i);
        pointLightsLocation[i].Atten.Exp = prog->addUniform(nameBuffer);
    }
    
    for (int i = 0 ; i < MAX_SPOT_LIGHTS; i++) {
        char nameBuffer[128];
        memset(nameBuffer, 0, sizeof(nameBuffer));
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Base.Base.Color", i);
        spotLightsLocation[i].Color = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Base.Base.AmbientIntensity", i);
        spotLightsLocation[i].AmbientIntensity = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Base.Position", i);
        spotLightsLocation[i].Position = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Direction", i);
        spotLightsLocation[i].Direction = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Cutoff", i);
        spotLightsLocation[i].Cutoff = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Base.Base.DiffuseIntensity", i);
        spotLightsLocation[i].DiffuseIntensity = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Base.Atten.Constant", i);
        spotLightsLocation[i].Atten.Constant = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Base.Atten.Linear", i);
        spotLightsLocation[i].Atten.Linear = prog->addUniform(nameBuffer);
        
        snprintf(nameBuffer, sizeof(nameBuffer), "spotLights[%d].Base.Atten.Exp", i);
        spotLightsLocation[i].Atten.Exp = prog->addUniform(nameBuffer);
    }
}
