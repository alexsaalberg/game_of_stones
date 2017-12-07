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
    void createLight(enum LightTypes lightType, glm::vec3 position, glm::vec3 invertedDirection, int windowWidth, int windowHeight);
    
    void bindForWritingAndClearDepthBuffer();
    
private:
    //Variables
    LightTypes lightType;
    glm::vec3 position;
    glm::vec3 invertedDirection;
    
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    
    int windowWidth;
    int windowHeight;
    
    GLuint frameBuffer;
    GLuint shadowDepthTexture;
    
    //Functions
    void generateAndLinkFBOAndTexture();
};


#endif /* Light_hpp */
