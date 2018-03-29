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

#include "WindowManager.h" //EventCallbacks
#include "Camera.hpp"

class System {
    
};

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
    
    void setMaterial(const std::shared_ptr<Program> prog, int i);
};

class Input_System : System, public EventCallbacks {
public:
    std::shared_ptr<EntityManager> entity_manager;
    
    //Control Callbacks
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void scrollCallback(GLFWwindow* window, double deltaX, double deltaY);
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void resizeCallback(GLFWwindow *window, int width, int height);
};

#endif /* System_h */
