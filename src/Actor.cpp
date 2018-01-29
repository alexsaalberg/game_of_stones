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

std::shared_ptr<Actor> Actor::interpolate(std::shared_ptr<Actor> &previous, std::shared_ptr<Actor> &current, float alpha) {
    shared_ptr<Actor> actor;
    actor = make_shared<Actor>();
    
    actor->position = current->position*alpha + previous->position*(1-alpha);
    actor->velocity = current->velocity*alpha + previous->velocity*(1-alpha);
    
    return actor;
}

vec3 Actor::calculateAcceleration(float t) {
    vec3 acceleration = vec3(0.0f); // meters per second per second
    
    acceleration.y = -9.8f;
    
    
    return acceleration;
}


void Actor::integrate(float t, float dt) {
    vec3 acceleration = calculateAcceleration( t );
    
    velocity += acceleration * dt;
    position += velocity * dt;
    
    if( actorIsOnGround() ) {
        velocity.y = 0.0f;
        position.y = calculateGroundHeight();
    }
    
    if(abs(position.x) > gridDistanceFromCenter)
        velocity.x *= -1.0f;
    
    if(abs(position.z) > gridDistanceFromCenter)
        velocity.z *= -1.0f;
}

bool Actor::actorIsOnGround() {
    if(position.y <= calculateGroundHeight() ) {
        return true;
    }
    return false;
}

float Actor::calculateGroundHeight() {
    return gridHeight+radius*2.0f;
}

void Actor::createActor(std::shared_ptr<Model> inModel) {
    position = vec3(0);
    rotation = vec3(0);
    
    velocity = vec3(0);
    omega = vec3(0.0f);
    
    scale = 1.0f;
    
    models.push_back(inModel);
    
    material = 1;
}

void Actor::step(double dt) {
    double framerate = 60.0f;
    dt *= framerate;
    //Position Physics
    //velocity.x *= pFrictionMult;
    //velocity.z *= pFrictionMult;
    
    velocity.x = clampVelocity(velocity.x, pVelocityMin);
    velocity.z = clampVelocity(velocity.z, pVelocityMin);
    
    position += velocity * (float) dt;
    
    if(position.y <= gridHeight+radius*2.0f) {
        velocity.y = 0.0f;
        position.y = gridHeight+radius*2.0f;
        if(captured)
            material = 6;
    } else {
        velocity.y -= gAcc;
    }
    
    
    if(abs(position.x) > gridDistanceFromCenter)
        velocity.x *= -1.0f;
    
    if(abs(position.z) > gridDistanceFromCenter)
        velocity.z *= -1.0f;
    
    
    
    //Rotation Physics
    omega *= rFrictionMult * dt;
    
    omega.x = clampVelocity(omega.x, rOmegaMin);
    omega.y = clampVelocity(omega.y, rOmegaMin);
    omega.z = clampVelocity(omega.z, rOmegaMin);
    
    rotation += omega * (float) dt;
    
    /*
    if(omega.x < rOmegaMin)
        omega.x = 0.0f;
    if(omega.y < rOmegaMin)
        //eOmega.y = 0.0f;
        if(omega.z < rOmegaMin)
            omega.z = 0.0f;*/
    
    //Collision Cooldown
    if(collisionCooldown) {
        collisionCooldown -= dt;
        if(collisionCooldown < 0.0f) {
            collisionCooldown = 0.0f;
        }
        
        if(collisionCooldown == 0.0f) {
            material = 2;
        }
    }
    
}

void Actor::draw(const std::shared_ptr<Program> prog) const {
    
    auto M = make_shared<MatrixStack>();
    
    M->pushMatrix();
    M->loadIdentity();
    
    
    
    M->translate(position);
    
    M->scale(scale);
    
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

