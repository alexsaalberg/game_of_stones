//
//  PlayerbirdPhysicsComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/16/18.
//

#ifndef PlayerbirdPhysicsComponent_hpp
#define PlayerbirdPhysicsComponent_hpp

#include "Component.hpp"
#include "glm/glm.hpp"
#include <string.h>

class PlayerbirdPhysicsComponent : public PhysicsComponent
{
public:
    virtual void update(GameObject& gameObject, float dt);
};

#endif /* PlayerbirdPhysicsComponent_hpp */
