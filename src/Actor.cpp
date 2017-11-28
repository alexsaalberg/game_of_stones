//
//  Actor.cpp
//  project4
//
//  Created by Alex Saalberg on 11/12/17.
//
//

#include "Actor.h"

#define PI 3.14
#define TORAD (PI / 180.0f)

using namespace std;
using namespace glm;
using namespace tinyobj;

const float gAcc = 0.01f;
const float pFrictionMult = 0.9f;
const float rFrictionMult = 0.9f;

const float pVelocityMin = 0.01f;
const float rOmegaMin = 1.0f;

void Actor::createActor(std::shared_ptr<Model> inModel) {
    ePosition = vec3(0);
    eRotation = vec3(0);
    
    eVelocity = vec3(0);
    eOmega = vec3(0.0f);
    eOmega.y = 10.0f;
    
    eVelocity = vec3(0.1f);
    eVelocity.z = 0.2f;
    
    eModels.push_back(inModel);
    
    material = 1;
}

void Actor::step() {
    ePosition += eVelocity;
    
    //Make sure the character is above the ground
    
    
    eVelocity.x *= pFrictionMult;
    eVelocity.z *= pFrictionMult;
    
    if(eVelocity.x > 0) {
        if(eVelocity.x < pVelocityMin)
            eVelocity.x = 0.0f;
    } else {
        if(eVelocity.x > -pVelocityMin)
            eVelocity.x = 0.0f;
    }
    
    if(eVelocity.z > 0) {
        if(eVelocity.z < pVelocityMin)
            eVelocity.z = 0.0f;
    } else {
        if(eVelocity.z > -pVelocityMin)
            eVelocity.z = 0.0f;
    }
    
    if(ePosition.y <= 0.0) {
        eVelocity.y = 0.0;
        ePosition.y = 0.0;
    } else {
        eVelocity.y -= gAcc;
    }
    
    eRotation += eOmega;
    //eOmega *= rFrictionMult;
    
    if(eOmega.x < rOmegaMin)
        eOmega.x = 0.0f;
    if(eOmega.y < rOmegaMin)
        //eOmega.y = 0.0f;
        if(eOmega.z < rOmegaMin)
            eOmega.z = 0.0f;
    
}

void Actor::setPosition(vec3 newPosition) {
    ePosition = newPosition;
}

void Actor::addVelocity(vec3 deltaVelocity) {
    eVelocity += deltaVelocity;
}

void Actor::addOffset(vec3 deltaOffset) {
    eOffset += deltaOffset;
}

void Actor::draw(const std::shared_ptr<Program> prog) const {
    
    auto M = make_shared<MatrixStack>();
    
    M->pushMatrix();
    M->loadIdentity();
    
    
    M->translate(ePosition);
    
    M->rotate(radians(eRotation.x), vec3(1, 0, 0));
    M->rotate(radians(eRotation.y), vec3(0, 1, 0));
    M->rotate(radians(eRotation.z), vec3(0, 0, 1));
    
    M->translate(eOffset);
    
    for (auto &model : eModels) // access by reference to avoid copying
    {
        model->draw(prog, M);
    }
    
    M->popMatrix();
}

