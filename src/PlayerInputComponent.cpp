//
//  PlayerInputComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "PlayerInputComponent.hpp"

using namespace glm;

void PlayerInputComponent::update(GameObject& gameObject, float dt) {
    //if(!movingUpward && !movingDownward && !movingLeftward && !movingRightward)
        //return;
    
    b2Vec2 worldVelocity = b2Vec2(20.0f, 2.0f);
    //Max velocity is relative to world ^^^
    
    b2Vec2 currentVelocity = gameObject.body->GetLinearVelocity();
    b2Vec2 inputVelocity = b2Vec2(15.0f, 38.0f);
    b2Vec2 desiredVelocity = worldVelocity;
    float timeToFullSpeed = 0.05f;
    
    //       (time of this step)*         (speed for 1s acceleration)
    //b2Vec2 inputVelocity =  dt * (1.0f / timeToFullSpeed) * maxVelocity;
    
    if (movingUpward) {
        desiredVelocity.y += 1.0f * inputVelocity.y;
    } else if (movingDownward) {
        desiredVelocity.y += -1.0f * inputVelocity.y;
    }
    
    if (movingRightward) {
        desiredVelocity.x += 1.0f * inputVelocity.x;
    } else if (movingLeftward) {
        desiredVelocity.x += -1.0f * inputVelocity.x;
    }
    
    b2Vec2 deltaAcceleration = (dt / timeToFullSpeed) * (desiredVelocity - currentVelocity);
    b2Vec2 appliedForce = gameObject.body->GetMass() * deltaAcceleration;
    
    gameObject.body->ApplyForceToCenter(appliedForce, true);
}
