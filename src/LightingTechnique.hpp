//
//  LightingTechnique.hpp
//  final471project
//
//  Created by Alex Saalberg on 12/7/17.
//  http://ogldev.atspace.co.uk/www/tutorial24/tutorial24.html
//

#ifndef LightingTechnique_hpp
#define LightingTechnique_hpp

#include <stdio.h>
#include "Program.h"
#include "GLSL.h"

class LightingTechnique {
public:
    static const unsigned int MAX_POINT_LIGHTS = 2;
    static const unsigned int MAX_SPOT_LIGHTS = 2;
    
    void createLightingTechnique(std::shared_ptr<Program> prog);
    
    void setDirectionalLight();
    void setPointLights();
    void setSpotLights();
    
private:
    
    GLuint numPointLightsLocation;
    GLuint numSpotLightsLocation;
    
    
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Direction;
    } dirLightLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } pointLightsLocation[MAX_POINT_LIGHTS];
    
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint Cutoff;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } spotLightsLocation[MAX_SPOT_LIGHTS];
};

#endif /* LightingTechnique_hpp */
