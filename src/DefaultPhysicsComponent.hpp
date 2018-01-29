//
//  DefaultPhysicsComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef DefaultPhysicsComponent_hpp
#define DefaultPhysicsComponent_hpp

#include <stdio.h>

#include "Component.hpp"
#include "glm/glm.hpp"

class DefaultPhysicsComponent : public PhysicsComponent
{
public:
    virtual void update(GameObject& gameObject, float dt);
    void integrate(GameObject& gameObject, float dt);
    
    glm::vec3 calculateAcceleration();
    glm::vec3 calculateImpulse();
};

#endif /* DefaultPhysicsComponent_hpp */
