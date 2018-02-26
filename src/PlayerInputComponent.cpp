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
    
    const float moveSpeed = 1.0f;
    /*
    if (movingUpward) {
        gameObject.impulse += vec3(0.0f, moveSpeed, 0.0f);
    } else if (movingDownward) {
        gameObject.impulse += vec3(0.0f, -1.0f * moveSpeed, 0.0f);
    }*/
}
