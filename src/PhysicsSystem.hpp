//
//  PhysicsSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#ifndef PhysicsSystem_hpp
#define PhysicsSystem_hpp

#include <memory> //shared_ptr

#include "System.hpp"
#include "EntityManager.hpp"

class PhysicsSystem : StepSystem {
public:
    virtual void step(double time, double delta_time);
    std::shared_ptr<EntityManager> entity_manager;
};

#endif /* PhysicsSystem_hpp */
