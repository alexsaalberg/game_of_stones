//
//  PlayerbirdPhysicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/16/18.
//

#include "PlayerbirdPhysicsComponent.hpp"

using namespace std;
using namespace glm;

void PlayerbirdPhysicsComponent::update(GameObject& gameObject, float dt) {
    if(gameObject.collisionCooldown == 0.0f) {
        for (b2ContactEdge* c = gameObject.body->GetContactList(); c != nullptr; c = c->next)
        {
            b2Body *other = c->other;
            void *userData = other->GetUserData();
            if(userData) {
                char *cString = (char *) userData;
                if(strcmp(cString, "bird") == 0) {
                    //other->SetActive(false);
                    //other->SetUserData((void *) "hit");
                }
                if(strcmp(cString, "blimp") == 0) {
                    //other->SetActive(false);
                }
            }
        }
    }
    
    /*
     if( gameObject.body->GetContactList() != nullptr ) {
     
     //printf("Health %d, Contact this step! %f\n", gameObject.health, dt);
     if(gameObject.collisionCooldown == 0.0f) {
     gameObject.collisionCooldown = 2.0f;
     gameObject.health -= 1;
     printf("Health %d\n", gameObject.health);
     }
     }
     */
}
