//
//  PlayerSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/20/18.
//

#ifndef PlayerSystem_hpp
#define PlayerSystem_hpp

#include <btRigidBody.h>

#include "System.hpp"
#include "EntityManager.hpp"

#include "CastleDef.h"

enum PlayerState {FPS_MODE, RTS_MODE};

class PlayerSystem : public System {
public:
    EntityId player_id;
    btRigidBody* player_body;
    
    //Virtual Functions
    virtual void init(const std::string& resourceDirectory);
    virtual void step(double t, double dt);
};

#endif /* PlayerSystem_hpp */
