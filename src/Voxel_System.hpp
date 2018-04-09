//
//  Voxel_System.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/7/18.
//

#ifndef Voxel_System_hpp
#define Voxel_System_hpp

#include <memory> //shared_ptr

#include "System.hpp"
#include "Messaging.h"
#include "EntityManager.hpp"
#include "WindowManager.h"

class Voxel_System : System, public Receiver<MouseClickEvent> {
public:
    WindowManager* window_manager;
    
    std::shared_ptr<EntityManager> entity_manager;
    
    void draw_voxels(float t, std::shared_ptr<Program> program);
    void setMVPE(std::shared_ptr<Program> program);
    
    virtual void receive(const MouseClickEvent& collision);
};

#endif /* Voxel_System_hpp */
