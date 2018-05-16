//
//  VolumeRenderSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/30/18.
//

#ifndef VolumeRenderSystem_hpp
#define VolumeRenderSystem_hpp

#include <memory> //shared_ptr

#include "System.hpp"
#include "Program.hpp"
#include "EntityManager.hpp"
#include "PolyVoxOpenGL.hpp"

class VolumeRenderSystem : FrameSystem {
public:
    std::shared_ptr<EntityManager> entity_manager;
    
    virtual void render(double t, std::shared_ptr<Program> program);
private:
    
    std::map<Entity_Id, PolyVoxExtensions::VolumeMeshData> model_map;
};

#endif /* VolumeRenderSystem_hpp */
