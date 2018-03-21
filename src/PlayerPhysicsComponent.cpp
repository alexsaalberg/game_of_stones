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
    if(gameObject.collisionCooldown == 0.0f) {
        for (b2ContactEdge* c = gameObject.body->GetContactList(); c != nullptr; c = c->next)
        {
            b2Body *other = c->other;
            void *userData = other->GetUserData();
            if(userData) {
                char *cString = (char *) userData;
                if(strcmp(cString, "blimp") == 0) {
                    //printf("blimp\n");
                    gameObject.collisionCooldown = 3.0f;
                    gameObject.health -=1 ;
                }
            }
        }
    }
}
