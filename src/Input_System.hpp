//
//  Input_System.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//
#ifndef Input_System_hpp
#define Input_System_hpp

#include "System.hpp"

#include "WindowManager.h" //EventCallbacks
#include "EntityManager.hpp"

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

#endif /* Input_System_hpp */
