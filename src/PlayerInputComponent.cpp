//
//  PlayerInputComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "PlayerInputComponent.hpp"

using namespace glm;

void PlayerInputComponent::update(GameObject& gameObject) {
    /*
    if (movingForward) {
        gameObject.impulse += gameObject.forwardDirection * -1.0f;
    } else if (movingBackward) {
        gameObject.impulse += gameObject.forwardDirection * +1.0f;
    }
    if (jumping) {
        gameObject.impulse.y += 2.0f;
        jumping = false;
    }*/
    
    const float moveSpeed = 10.0f;
    
    b2Vec2 desiredVelocity = b2Vec2(0.0f, 0.0f);
    
    if (movingUpward) {
        desiredVelocity.y = 1.0f * moveSpeed;
    } else if (movingDownward) {
        desiredVelocity.y = -1.0f * moveSpeed;
    }
    b2Vec2 velocityChange = desiredVelocity - gameObject.body->GetLinearVelocity();
    b2Vec2 desiredImpulse = gameObject.body->GetMass() * velocityChange;
    desiredImpulse.x = 0.0f;
    gameObject.body->ApplyLinearImpulseToCenter(desiredImpulse, true);
}
