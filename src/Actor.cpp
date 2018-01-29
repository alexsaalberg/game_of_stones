//
//  Actor.cpp
//  project4
//
//  Created by Alex Saalberg on 11/12/17.
//
//

#include "Actor.h"

#include "Helper.h"

using namespace std;
using namespace glm;
using namespace tinyobj;

void Actor::initActor(std::shared_ptr<Model> inModel) {
    models.push_back(inModel);
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


bool Actor::actorIsOnGround() {
    if(position.y <= calculateGroundHeight() ) {
        return true;
    }
    return false;
}

float Actor::calculateGroundHeight() {
    return gridHeight+radius*2.0f;
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

