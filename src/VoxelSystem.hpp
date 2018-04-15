//
//  VoxelSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/7/18.
//

#ifndef VoxelSystem_hpp
#define VoxelSystem_hpp

#include <memory> //shared_ptr

#include "System.hpp"
#include "Messaging.h"
#include "EntityManager.hpp"
#include "WindowManager.h"

class VoxelSystem : System, public Receiver<MouseClickEvent> {
public:
    //Variables
    WindowManager* window_manager;
    std::shared_ptr<EntityManager> entity_manager;
    std::vector<MouseClickEvent> click_events;
    
    //Functions
    void update(double t);
    void processClickEvent(double t, MouseClickEvent& click);
    virtual void receive(const MouseClickEvent& collision);
};

#endif /* VoxelSystem_hpp */
