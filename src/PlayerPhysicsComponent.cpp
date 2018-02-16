//
//  PlayerPhysicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 2/1/18.
//

#include "PlayerPhysicsComponent.hpp"

using namespace std;
using namespace glm;

void PlayerPhysicsComponent::update(GameObject& gameObject, float dt) {
    integrate(gameObject, dt);
}

void PlayerPhysicsComponent::integrate(GameObject& gameObject, float dt) {
    
    gameObject.velocity += gameObject.impulse;
    gameObject.impulse = vec3(0.0f);
    
    vec3 acceleration = calculateAcceleration(gameObject, dt);
    
    gameObject.velocity += acceleration * dt;
    gameObject.position += gameObject.velocity * dt;
    
    float groundHeight = calculateGroundHeight();
    
    /*
    if(gameObject.position.y < groundHeight ) {
        gameObject.velocity.y = 0.0f;
        gameObject.position.y = groundHeight;
    }*/
}

float PlayerPhysicsComponent::calculateGroundHeight() {
    return 1.0f;
}

vec3 PlayerPhysicsComponent::calculateAcceleration(GameObject& gameObject, float dt) {
    vec3 acceleration = vec3(0.0f); // meters per second per second
    //acceleration.y = -9.8f;
    
    float frictionMultiplier = 0.95f;
    
    float speedDiff = maxHorizontalSpeed - gameObject.velocity.x; //accelerate towards max speed
    if( abs(speedDiff) > 0.01f ) {
        acceleration.x = speedDiff;  //(should take ~1 sec to achieve max speed)
    }
    
    //acceleration.x = -1.0f * frictionMultiplier * gameObject.velocity.x;
    acceleration.y = -1.0f * frictionMultiplier * gameObject.velocity.y;
    
    const float maximumHeight = 10.0f; //BirdHighSpawnY = ~10.0
    float heightDiff = maximumHeight - gameObject.position.y;
    if( heightDiff < 0.0f ) { //heightDiff will be (-) if player above max
        acceleration.y += heightDiff * 20.0f;
    }
    
    const float minimumHeight = 0.0f; //BirdHighSpawnY = ~10.0
    heightDiff = minimumHeight - gameObject.position.y;
    if( heightDiff > 0.0f ) { //heightDiff will be (+) if player below min
        acceleration.y += heightDiff * 20.0f;
    }

    
    return acceleration;
}
