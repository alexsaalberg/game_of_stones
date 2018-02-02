//
//  PlayerPhysicsComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 2/1/18.
//

#ifndef PlayerPhysicsComponent_hpp
#define PlayerPhysicsComponent_hpp

#include <stdio.h>

#include "Component.hpp"
#include "glm/glm.hpp"

class PlayerPhysicsComponent : public PhysicsComponent
{
    const float maxHorizontalSpeed = 20.0f;
    
public:
    virtual void update(GameObject& gameObject, float dt);
    void integrate(GameObject& gameObject, float dt);
    
    float calculateGroundHeight();
    
    glm::vec3 calculateAcceleration(GameObject& gameObject, float dt);
};


#endif /* PlayerPhysicsComponent_hpp */
