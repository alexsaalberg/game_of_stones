//
//  PlayerInputComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "PlayerbirdInputComponent.hpp"

using namespace glm;

void PlayerbirdInputComponent::update(GameObject& gameObject, float dt) {
    if(timeToLive <= 0.0f) {
        timeToLive = 0.0f;
        return;
    }
    timeToLive -= dt;
    
    b2Vec2 worldVelocity = b2Vec2(0.0f, 0.0f);
    //Max velocity is relative to world ^^^
    
    b2Vec2 currentVelocity = gameObject.body->GetLinearVelocity();
    b2Vec2 inputVelocity = b2Vec2(10.0f, 10.0f);
    b2Vec2 desiredVelocity = currentVelocity;
    float timeToFullSpeed = 0.01f;
    
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
    float angle = atan(desiredVelocity.y / desiredVelocity.x);
    gameObject.body->SetTransform(gameObject.body->GetPosition(), angle);
}

