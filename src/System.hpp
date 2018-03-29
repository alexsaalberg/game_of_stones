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
