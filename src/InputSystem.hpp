//
//  InputSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//
#ifndef InputSystem_hpp
#define InputSystem_hpp

#include "System.hpp"

#include <string>

#include "WindowManager.h" //EventCallbacks

#include "EntityManager.hpp"
#include "Messaging.h"
#include "ChunkSystem.hpp"

/*
 
 In this file there's an important distinction between a "Control" and an "Input"
 
 An input is things like keyboard presses, mouse movement, etc.
 (All the stuff with callbacks basically)
 
 A control is addressed via a string ("Move_Forward") Control names must be unique
 This allows them to be remappable or multiple inputs to one control
 or multiple systems responding to one control or wahtever
 
 */


struct Control {
    int key;
    int mods;
    bool downThisStep;
    bool downLastStep;
    bool pressedThisStep;
};

class InputSystem : System, public EventCallbacks {
public:
    std::shared_ptr<EntityManager> entity_manager;
    std::shared_ptr<EventHandler> event_handler;
    ChunkSystem* chunk_system;
    
    std::map<std::string, Control> control_map;
    
    //Controls
    void addControl(std::string name, int key);
    Control getControl(std::string name);
    
    bool isControlDownThisStep(std::string name);
    bool wasControlDownLastStep(std::string name);
    
    //Update
    void update(double t);
    
    //Input Callbacks
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void scrollCallback(GLFWwindow* window, double deltaX, double deltaY);
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void resizeCallback(GLFWwindow *window, int width, int height);
};

#endif /* InputSystem_hpp */
