//
//  Light.hpp
//  final471project
//
//  Created by Alex Saalberg on 12/7/17.
//

#ifndef Light_hpp
#define Light_hpp

#include <stdio.h>
#include <iostream>

#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "glm/glm.hpp"


#include <glm/gtc/matrix_transform.hpp>

enum class LightTypes {
    SpotLight, //Cone shape
    PointLight //Directional, like The Sun
};

class Light {
public:
    void createLight(enum LightTypes lightType, int windowWidth, int windowHeight);
    
    void setPosition(glm::vec3 newPosition);
    void setInvertedDirection(glm::vec3 newInvertedDirection);
    void setLightType(enum LightTypes newLightType);
    
    void createMatrices();
    
    void bindForWritingAndClearDepthBuffer();
    void bindForReading(GLenum TextureUnit);

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    
    glm::vec3 invertedDirection;
    
private:
    //Variables
    LightTypes lightType;
    glm::vec3 position;
    
    
    int windowWidth;
    int windowHeight;
    
    GLuint frameBuffer;
    GLuint shadowDepthTexture;
    
    //Functions
    void generateAndLinkFBOAndTexture();
};


#endif /* Light_hpp */
