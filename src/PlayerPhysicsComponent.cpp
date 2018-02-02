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
    
    if(gameObject.position.y < groundHeight ) {
        gameObject.velocity.y = 0.0f;
        gameObject.position.y = groundHeight;
    }
}

float PlayerPhysicsComponent::calculateGroundHeight() {
    return 1.0f;
}

vec3 PlayerPhysicsComponent::calculateAcceleration(GameObject& gameObject, float dt) {
    vec3 acceleration = vec3(0.0f); // meters per second per second
    //acceleration.y = -9.8f;
    
    float frictionMultiplier = 0.95f;
    
    float diff = maxHorizontalSpeed - gameObject.velocity.x; //accelerate towards max speed
    if( abs(diff) > 0.01f ) {
        acceleration.x = diff;  //(should take ~1 sec to achieve max speed)
    }

    //acceleration.x = -1.0f * frictionMultiplier * gameObject.velocity.x;
    acceleration.y = -1.0f * frictionMultiplier * gameObject.velocity.y;
    
    return acceleration;
}
