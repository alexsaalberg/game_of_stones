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
enum SelectionState {BUILD_MODE, COLONIST_MODE};

class PlayerSystem : public System {
public:
    //Functions
    EntityId player_id;
    btRigidBody* player_body;
    
    PlayerState player_state = FPS_MODE;
    SelectionState selection_state = BUILD_MODE;
    
    //Virtual Functions
    virtual void init(const std::string& resourceDirectory);
    virtual void step(double t, double dt);
private:
    void fps_step(double t, double dt);
    void rts_step(double t, double dt);
};

#endif /* PlayerSystem_hpp */
