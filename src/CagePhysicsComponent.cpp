//
//  CagePhysicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/16/18.
//

#include "CagePhysicsComponent.hpp"

using namespace std;
using namespace glm;

void CagePhysicsComponent::update(GameObject& gameObject, float dt) {
    if(gameObject.collisionCooldown == 0.0f) {
        for (b2ContactEdge* c = gameObject.body->GetContactList(); c != nullptr; c = c->next)
        {
            b2Body *other = c->other;
            void *userData = other->GetUserData();
            if(userData) {
                char *cString = (char *) userData;
                if(strcmp(cString, "bird") == 0) {
                    //printf("bird collision\n");
                    if(gameObject.health > 0) {
                        gameObject.score += 1;
                    }
                    other->SetTransform(b2Vec2(0.0, 0.0), 0.0);
                    other->SetUserData((void *) "hit");
                }
            }
        }
    }
}
