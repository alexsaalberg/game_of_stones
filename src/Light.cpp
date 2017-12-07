//
//  Light.cpp
//  final471project
//
//  Created by Alex Saalberg on 12/7/17.
//

#include "Light.hpp"

using namespace std;
using namespace glm;

void Light::createLight(enum LightTypes lightType, int windowWidth, int windowHeight) {
    this->lightType = lightType;
    
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    
    //Requires width & height to be set!
    generateAndLinkFBOAndTexture();
}

void Light::setPosition(vec3 newPosition) {
    position = newPosition;
}

void Light::setInvertedDirection(vec3 newInvertedDirection) {
    invertedDirection = newInvertedDirection;
}

void Light::setLightType(enum LightTypes newLightType) {
    lightType = newLightType;
}

void Light::createMatrices() {
    model = glm::mat4();
    switch(lightType) {
        case LightTypes::SpotLight: //Cone shape
            projection = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
            view = glm::lookAt(position, position - invertedDirection, vec3(0, 1, 0));
            break;
        case LightTypes::PointLight: //Far away, parallel rays
            projection = glm::ortho<float>(-10,10,-10,10,-10,20);
            view = glm::lookAt(position, vec3(0, 0, 0), vec3(0, 1, 0));
            break;
    }
}

void Light::bindForWritingAndClearDepthBuffer() {
    CHECKED_GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer));
    CHECKED_GL_CALL(glViewport(0,0,windowWidth,windowHeight));
    CHECKED_GL_CALL(glClear(GL_DEPTH_BUFFER_BIT));
}

void Light::bindForReading(GLenum TextureUnit) {
    CHECKED_GL_CALL(glActiveTexture(TextureUnit));
    CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, shadowDepthTexture));
}
    
void Light::generateAndLinkFBOAndTexture() {
    //Generate and bind FBO
    CHECKED_GL_CALL(glGenFramebuffers(1, &frameBuffer));
    CHECKED_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer));
    
    //Generate and bind Texture
    CHECKED_GL_CALL(glGenTextures(1, &shadowDepthTexture));
    CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, shadowDepthTexture));
    
    
    CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, windowWidth, windowHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0));
    CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    
    //Unsure
    CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE));
    
    //Set 'shadowDepthBuffer' to be the data storage for 'shadowFramebuffer'
    CHECKED_GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0));
    
    // No color output in the bound framebuffer, only depth
    CHECKED_GL_CALL(glDrawBuffer(GL_NONE));
    
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "shadowFramebuffer not okay!" << std::endl;
        exit(1);
    }
}
