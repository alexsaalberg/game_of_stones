//
//  PlayerInputComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "PlayerInputComponent.hpp"

void PlayerInputComponent::update(GameObject& gameObject) {
    if (movingForward) {
        gameObject.impulse += gameObject.forwardDirection * 1.0f;
    } else if (movingBackward) {
        gameObject.impulse -= gameObject.forwardDirection * -1.0f;
    }
}
