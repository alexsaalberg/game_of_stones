//
//  Actor.cpp
//  project4
//
//  Created by Alex Saalberg on 11/12/17.
//
//

#include "Actor.h"

#include "Helper.h"

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
    position = vec3(0);
    rotation = vec3(0);
    
    velocity = vec3(0);
    omega = vec3(0.0f);
    
    aScale = 1.0f;
    
    models.push_back(inModel);
    
    material = 1;
}

void Actor::step() {
    //Position Physics
    position += velocity;
    
    velocity.x *= pFrictionMult;
    velocity.z *= pFrictionMult;
    
    velocity.x = clampVelocity(velocity.x, pVelocityMin);
    velocity.z = clampVelocity(velocity.z, pVelocityMin);
    
    if(position.y <= 0.0) {
        velocity.y = 0.0;
        position.y = 0.0;
    } else {
        velocity.y -= gAcc;
    }
    
    //Rotation Physics
    rotation += omega;
    omega *= rFrictionMult;
    
    rotation += vec3(0.0f, 0.0f, 1.0f);
    
    omega.x = clampVelocity(omega.x, rOmegaMin);
    omega.y = clampVelocity(omega.y, rOmegaMin);
    omega.z = clampVelocity(omega.z, rOmegaMin);
    /*
    if(omega.x < rOmegaMin)
        omega.x = 0.0f;
    if(omega.y < rOmegaMin)
        //eOmega.y = 0.0f;
        if(omega.z < rOmegaMin)
            omega.z = 0.0f;*/
    
}

void Actor::setPosition(vec3 newPosition) {
    position = newPosition;
}
vec3 Actor::getPosition() {
    return position;
}

void Actor::setRotation(vec3 newRotation) {
    rotation = newRotation;
}
void Actor::addRotation(vec3 deltaRotation) {
    rotation += deltaRotation;
}
vec3 Actor::getRotation() {
    return rotation;
}

void Actor::addVelocity(vec3 deltaVelocity) {
    velocity += deltaVelocity;
}

void Actor::addOffset(vec3 deltaOffset) {
    modelOffset += deltaOffset;
}

void Actor::scale(float deltaScale) {
    aScale *= deltaScale;
}

float Actor::getScale() {
    return aScale;
}

void Actor::draw(const std::shared_ptr<Program> prog) const {
    
    auto M = make_shared<MatrixStack>();
    
    M->pushMatrix();
    M->loadIdentity();
    
    
    
    M->translate(position);
    
    M->scale(aScale);
    
    M->rotate(radians(rotation.x), vec3(1, 0, 0));
    M->rotate(radians(rotation.y), vec3(0, 1, 0));
    M->rotate(radians(rotation.z), vec3(0, 0, 1));

    M->translate(modelOffset);
    
    
    for (auto &model : models) // access by reference to avoid copying
    {
        model->draw(prog, M);
    }
    
    M->popMatrix();
}
