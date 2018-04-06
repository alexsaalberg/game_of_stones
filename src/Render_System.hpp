//
//  Render_System.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//

#ifndef Render_System_h
#define Render_System_h

#include "System.hpp"

#include <memory> //shared_ptr

#include "EntityManager.hpp"
#include "WindowManager.h" //WindowManager
#include "Program.h"

#include "PolyVox_OpenGL.hpp"
#include "PolyVoxExample.h"

class Render_System : System {
public:
    WindowManager* window_manager;
    
    void draw(std::shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program);
    void draw_entities(std::shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program);
    void draw_voxels(std::shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program);
    
    //Voxel Stuff
    void initVoxels();
    std::shared_ptr< PolyVox::RawVolume<uint8_t> > volData;
    std::shared_ptr< PolyVox::PagedVolume<uint8_t> > pagedData;
    PolyVox_OpenGL voxel_rend;
    PolyVoxExample poly_vox_example;
    
    void createFunction(PolyVox::RawVolume<uint8_t>& volData);
    void createFunction3D(PolyVox::RawVolume<uint8_t>& volData);
    void clearRegion(PolyVox::PagedVolume<uint8_t>& volData, PolyVox::Region region);
    void createLand(PolyVox::PagedVolume<uint8_t>& volData, PolyVox::Region region);
    void createLand(PolyVox::RawVolume<uint8_t>& volData);
    void createSphereInVolume(PolyVox::RawVolume<uint8_t>& volData, float fRadius);
    
    //Camera Functions
    void setMVPE(std::shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program);
    
    void setModelIdentityMatrix(std::shared_ptr<Program> prog);
    
    std::shared_ptr<MatrixStack> getViewMatrix(Camera_Component* camera);
    void setViewMatrix(Camera_Component* camera, std::shared_ptr<Program> prog);
    
    std::shared_ptr<MatrixStack> getProjectionMatrix(float aspect);
    void setProjectionMatrix(std::shared_ptr<Program> prog, float aspect);
    
    void setEyePosition(Camera_Component* camera, std::shared_ptr<Program> prog);
    
    void renderGUI();
    
    void setMaterial(const std::shared_ptr<Program> prog, int i);
};

#endif /* Render_System_h */
