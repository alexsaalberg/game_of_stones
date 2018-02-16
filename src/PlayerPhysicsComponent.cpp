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
    if( gameObject.body->GetContactList() != nullptr ) {
        //printf("Health %d, Contact this step! %f\n", gameObject.health, dt);
        if(gameObject.collisionCooldown == 0.0f) {
            gameObject.collisionCooldown = 2.0f;
            gameObject.health -= 1;
            printf("Health %d\n", gameObject.health);
        }
    }
}
