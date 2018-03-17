//
//  CagePhysicsComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/16/18.
//

#ifndef CagePhysicsComponent_hpp
#define CagePhysicsComponent_hpp

#include "Component.hpp"
#include "glm/glm.hpp"
#include <string.h>

class CagePhysicsComponent : public PhysicsComponent
{
public:
    virtual void update(GameObject& gameObject, float dt);
};
#endif /* CagePhysicsComponent_hpp */
