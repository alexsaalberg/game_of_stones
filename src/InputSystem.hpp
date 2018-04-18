//
//  InputSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//
#ifndef InputSystem_hpp
#define InputSystem_hpp

#include "System.hpp"

#include "WindowManager.h" //EventCallbacks

#include "EntityManager.hpp"
#include "Messaging.h"
#include "ChunkSystem.hpp"

class InputSystem : System, public EventCallbacks {
public:
    std::shared_ptr<EntityManager> entity_manager;
    std::shared_ptr<EventHandler> event_handler;
    ChunkSystem* chunk_system;
    
    //Control Callbacks
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void scrollCallback(GLFWwindow* window, double deltaX, double deltaY);
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void resizeCallback(GLFWwindow *window, int width, int height);
};

#endif /* InputSystem_hpp */
