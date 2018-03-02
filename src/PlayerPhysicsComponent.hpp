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
public:
    virtual void update(GameObject& gameObject, float dt);
};


#endif /* PlayerPhysicsComponent_hpp */ 
