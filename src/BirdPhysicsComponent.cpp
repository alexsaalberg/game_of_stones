//
//  BirdPhysicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 2/1/18.
//

#include "BirdPhysicsComponent.hpp"

using namespace std;
using namespace glm;

void BirdPhysicsComponent::update(GameObject& gameObject, float dt) {
    integrate(gameObject, dt);
}

void BirdPhysicsComponent::integrate(GameObject& gameObject, float dt) {
    
    gameObject.velocity += gameObject.impulse;
    gameObject.impulse = vec3(0.0f);
    
    vec3 acceleration = calculateAcceleration(gameObject, dt);
    
    gameObject.velocity += acceleration * dt;
    gameObject.position += gameObject.velocity * dt;
}

float BirdPhysicsComponent::calculateGroundHeight() {
    return 1.0f;
}

vec3 BirdPhysicsComponent::calculateAcceleration(GameObject& gameObject, float dt) {
    return vec3(0.0f);
}
