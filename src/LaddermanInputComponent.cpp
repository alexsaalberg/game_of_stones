//
//  PlayerInputComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "LaddermanInputComponent.hpp"

using namespace glm;

void LaddermanInputComponent::update(GameObject& gameObject, float dt) {
    b2Vec2 moveSpeed = b2Vec2(10.0f, 10.0f);
    float move = 10.0f;
    
    //b2Vec2 desiredVelocity = b2Vec2(0.0f, 0.0f);
    //b2Vec2 velocityChange = desiredVelocity - gameObject.body->GetLinearVelocity();
    //b2Vec2 desiredImpulse = gameObject.body->GetMass() * velocityChange;
    b2Vec2 desiredAcceleration = b2Vec2(0.0f, 0.0f);
    b2Vec2 velocityChange = b2Vec2(0.0f, 0.0f);
    b2Vec2 desiredImpulse = b2Vec2(0.0f, 0.0f);
    float angle = gameObject.body->GetAngle();
    b2Vec2 normal = b2Vec2(cos(angle), sin(angle));
    
    if (movingUpward) {
    } else if (movingDownward) {
    }
    
    if (movingRightward) {
        desiredAcceleration = 1.0f * normal;
    } else if (movingLeftward) {
        desiredAcceleration = -1.0f * normal;
        printf("Normal X:%f Y:%f\n", normal.x, normal.y);
    }
    
    
    velocityChange = desiredAcceleration - gameObject.body->GetLinearVelocity();
    desiredImpulse = gameObject.body->GetMass() * velocityChange;
    //gameObject.body->ApplyLinearImpulseToCenter(desiredImpulse, true);
    desiredAcceleration *= gameObject.body->GetMass();
    desiredAcceleration *= move;
    gameObject.body->ApplyForceToCenter(desiredAcceleration, true);
}

