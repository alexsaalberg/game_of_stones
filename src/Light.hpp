//
//  Light.hpp
//  final471project
//
//  Created by Alex Saalberg on 12/7/17.
//

#ifndef Light_hpp
#define Light_hpp

#include <stdio.h>

#include <glad/glad.h>

#include "glm/glm.hpp"

enum class LightTypes {
    SpotLight, //Cone shape
    PointLight //Directional, like The Sun
};

class Light {
public:
    void createLight(LightTypes lightType);
private:
    glm::vec3 position;
    glm::vec3 direction;
    
    GLuint frameBuffer;
    GLuint shadowDepthTexture;
    
    LightTypes lightType;
    
};


#endif /* Light_hpp */
