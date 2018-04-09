//
//  Physics_System.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#ifndef Physics_System_hpp
#define Physics_System_hpp

#include <memory> //shared_ptr

#include "System.hpp"
#include "EntityManager.hpp"

class Physics_System : System {
public:
    std::shared_ptr<EntityManager> entity_manager;
};

#endif /* Physics_System_hpp */
