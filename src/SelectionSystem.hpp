//
//  SelectionSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/25/18.
//

#ifndef SelectionSystem_hpp
#define SelectionSystem_hpp

#include <memory>

#include "System.hpp"

#include "Program.h"

#include "EntityManager.hpp"

class SelectionSystem : StepSystem {
public:
    std::shared_ptr<EntityManager> entity_manager;
    
    virtual void step(double t, double dt);
    //void render(double t, std::shared_ptr<Program> program);
};

#endif /* SelectionSystem_hpp */
