//
//  BirdPhysicsComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 2/1/18.
//

#ifndef BirdPhysicsComponent_hpp
#define BirdPhysicsComponent_hpp

#include <stdio.h>

#include "Component.hpp"
#include "glm/glm.hpp"

class BirdPhysicsComponent : public PhysicsComponent
{
public:
    virtual void update(GameObject& gameObject, float dt);
    void integrate(GameObject& gameObject, float dt);
    
    float calculateGroundHeight();
    
    glm::vec3 calculateAcceleration(GameObject& gameObject, float dt);
};

#endif /* BirdPhysicsComponent_hpp */
