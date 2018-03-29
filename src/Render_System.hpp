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

class Render_System : System {
public:
    WindowManager* window_manager;
    
    void draw(std::shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program);
    void draw_entities(std::shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program);
    
    //Camera Functions
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
