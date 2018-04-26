//
//  PickSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/24/18.
//

#ifndef PickSystem_hpp
#define PickSystem_hpp

#include <memory>

#include "EntityManager.hpp"
#include "ChunkSystem.hpp"
#include "InputSystem.hpp"

#include "PolyVox/Vector.h"

#include "CastleDef.h"

class PickSystem {
public:
    Entity_Id cursor_id;
    Entity_Id selection_id;
    std::shared_ptr<EntityManager> entity_manager;
    InputSystem* input_system;
    ChunkSystem* chunk_system;
    
    static Region createProperRegion(Region& region); //creates a region where lowerXYZ is < upperXYZ
    
    void update(double t);
    
    void deleteRegion(double t, Region& region);
    Vector3DInt32 pickScreen(float screenX, float screenY, bool previous);
    
};

#endif /* PickSystem_hpp */
