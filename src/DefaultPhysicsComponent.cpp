//
//  DefaultPhysicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "DefaultPhysicsComponent.hpp"

using namespace std;
using namespace glm;

void DefaultPhysicsComponent::update(GameObject& gameObject, float dt) {
    integrate(gameObject, dt);
}

void DefaultPhysicsComponent::integrate(GameObject& gameObject, float dt) {
    vec3 acceleration = calculateAcceleration();
    vec3 impulse = calculateImpulse();
    
    gameObject.velocity += acceleration * dt;
    gameObject.position += gameObject.velocity * dt;
    
    /*
    if( actorIsOnGround() ) {
        gameObject.velocity.y = 0.0f;
        gameObject.position.y = calculateGroundHeight();
    } */
}

vec3 DefaultPhysicsComponent::calculateAcceleration() {
    vec3 acceleration = vec3(0.0f); // meters per second per second
    acceleration.y = -9.8f;
    return acceleration;
}

vec3 DefaultPhysicsComponent::calculateImpulse() {
    return vec3(0.0f);
}
