//
//  System.h
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#ifndef System_h
#define System_h

#include <memory> //shared_ptr

#include "EntityManager.hpp"
#include "Component_Storage.hpp"
#include "Program.h"

class System {
    
};

class Render_System : System {
public:
    Component_Storage<Renderable_Component> manager;
    
    void draw(std::shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program);
};

class Input_System : System {
public:
    Component_Storage<Camera_Component> camera_manager;
    Component_Storage<Player_Component> player_manager;
};

#endif /* System_h */
