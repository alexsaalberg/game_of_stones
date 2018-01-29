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
    
    gameObject.velocity += gameObject.impulse;
    gameObject.impulse = vec3(0.0f);
    
    vec3 acceleration = calculateAcceleration(gameObject, dt);
    
    gameObject.velocity += acceleration * dt;
    gameObject.position += gameObject.velocity * dt;
    
    float groundHeight = calculateGroundHeight();
    
    if(gameObject.position.y < groundHeight ) {
        gameObject.velocity.y = 0.0f;
        gameObject.position.y = groundHeight;
    }
}

float DefaultPhysicsComponent::calculateGroundHeight() {
    return 1.0f;
}

vec3 DefaultPhysicsComponent::calculateAcceleration(GameObject& gameObject, float dt) {
    vec3 acceleration = vec3(0.0f); // meters per second per second
    acceleration.y = -9.8f;
    
    float frictionMultiplier = 0.95f;
    acceleration.x = -1.0f * frictionMultiplier * gameObject.velocity.x;
    
    return acceleration;
}
