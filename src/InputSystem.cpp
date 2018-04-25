//
//  InputSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//

#include "InputSystem.hpp"

#include <vector>

#include "MatrixStack.h"
#include "PolyVox/Picking.h"

using namespace std;
using namespace glm;

void InputSystem::update(double t) {
    for(auto& map_entry : control_map) {
        map_entry.second.previousValue = map_entry.second.currentValue;
        //map_entry.second.currentValue = 0.0f;
        map_entry.second.pressedLastStep = map_entry.second.pressedThisStep;
        map_entry.second.pressedThisStep = false;
    }
    
    glfwPollEvents();
}

void InputSystem::addKeyControl(string name, int key) {
    Control control;
    control.type = INPUT_KEYBOARD;
    control.key = key;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}

void InputSystem::addMouseclickControl(string name, int button) {
    Control control;
    control.type = INPUT_MOUSE_BUTTON;
    control.key = button;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}
void InputSystem::addMouseposXControl(string name) {
    Control control;
    control.type = INPUT_MOUSE_POSITION_X;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}

void InputSystem::addMouseposYControl(string name) {
    Control control;
    control.type = INPUT_MOUSE_POSITION_Y;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}

Control InputSystem::getControl(std::string name) {
    Control control;
    
    auto iterator = control_map.find(name);
    if(iterator != control_map.end()) {
        control = control_map.at(name);
    } else {
        //Control doesn't exist....
        control.key = GLFW_KEY_UNKNOWN;
        printf("Unknown control: %s\n", name.c_str());
    }
    return control;
}

bool InputSystem::isControlDownThisStep(std::string name) {
    const Control& control = getControl(name);
    
    if(control.currentValue > press_threshold) {
        return true;
    }
    
    return false;
}

bool InputSystem::wasControlDownLastStep(std::string name) {
    const Control& control = getControl(name);
    
    if(control.previousValue > press_threshold) {
        return true;
    }
    
    return false;
}

float InputSystem::getCurrentControlValue(std::string name) {
    const Control& control = getControl(name);
    
    return control.currentValue;
}

float InputSystem::getPreviousControlValue(std::string name) {
    const Control& control = getControl(name);
    
    return control.previousValue;
}

bool InputSystem::wasControlPressedThisStep(std::string name) {
    const Control& control = getControl(name);
    
    return control.pressedThisStep;
}
bool InputSystem::wasControlPressedLastStep(std::string name) {
    const Control& control = getControl(name);
    
    return control.pressedLastStep;
}

void InputSystem::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    for(auto& map_entry : control_map) {
        if(map_entry.second.type == INPUT_KEYBOARD) {
            if(map_entry.second.key == key) {
                if(action == GLFW_PRESS) {
                    map_entry.second.currentValue = 1.0f;
                    map_entry.second.pressedThisStep = true;
                }
                if(action == GLFW_RELEASE) {
                    map_entry.second.currentValue = 0.0f;
                }
            }
        }
    }
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Entity_Id camera_id = camera_ids.at(0);
    
    //Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_id);
    Position_Component* position = entity_manager->get_component<Position_Component>(camera_id);
    
    static float move_scale = 1.0f;
    static float rotate_scale = 1.0f;
    float delta_angle = 5.0;
    float delta_distance = 0.25f * move_scale;
    
    //positive z is forward (default orientation of camera)
    vec3 forwardMove = position->rotation * vec3(0.0f, 0.0f, 1.0f);
    vec3 rightwardMove = position->rotation * vec3(1.0f, 0.0f, 0.0f);
    
    forwardMove = normalize(forwardMove) * delta_distance;
    rightwardMove = normalize(rightwardMove) * delta_distance;
    
    forwardMove = vec3(forwardMove.x, 0.0f, forwardMove.z);
    rightwardMove = vec3(rightwardMove.x, 0.0f, rightwardMove.z);
    
    //printf("F(%f %f %f) R(%f %f %f)\n", forwardMove.x, forwardMove.y, forwardMove.z, rightwardMove.x, rightwardMove.y, rightwardMove.z);
    
    delta_angle = glm::radians(delta_angle);
    
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        move_scale *= 2.0f;
        printf("Move Scale: %f\n", move_scale);
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        move_scale /=2.0f;
        if(move_scale < 0.25f) {
            move_scale = 0.25f;
        }
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        rotate_scale *= 2.0f;
        printf("Move Scale: %f\n", move_scale);
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        rotate_scale /=2.0f;
        if(rotate_scale < 0.25f) {
            rotate_scale = 0.25f;
        }
    }
    
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        position->position += forwardMove;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        position->position -= forwardMove;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        position->position -= rightwardMove;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        position->position += rightwardMove;
    }
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        glm::quat deltaRotation;
        //deltaRotation = glm::quat( glm::vec3(glm::radians(-1.0f * delta_angle), 0.0f, 0.0f) );
        deltaRotation = glm::angleAxis(-1.0f * delta_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        //position->rotation.x -= delta_angle;
        position->rotation *= deltaRotation;
        //camera->phi += delta_angle;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        glm::quat deltaRotation;
        deltaRotation = glm::angleAxis(1.0f * delta_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        position->rotation *= deltaRotation;
        //camera->phi -= delta_angle;
    }
    
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        glm::quat deltaRotation;
        deltaRotation = glm::angleAxis(1.0f * delta_angle, glm::vec3(0.0f, 1.0f, 0.0f));
        position->rotation *= deltaRotation;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        glm::quat deltaRotation;
        deltaRotation = glm::angleAxis(-1.0f * delta_angle, glm::vec3(0.0f, 1.0f, 0.0f));
        position->rotation *= deltaRotation;
    }
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        position->position.y += delta_distance;
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        position->position.y -= delta_distance;
    }
}

//Todo: Remove these (Idk if they're being optimized out, but hopefully
//                    they're not being called every time the mouse moves)
void InputSystem::scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
{}
void InputSystem::mouseCallback(GLFWwindow *window, int button, int action, int mods)
{
    for(auto& map_entry : control_map) {
        if(map_entry.second.type == INPUT_MOUSE_BUTTON) {
            if(map_entry.second.key == button) {
                if(action == GLFW_PRESS) {
                    map_entry.second.currentValue = 1.0f;
                    map_entry.second.pressedThisStep = true;
                }
                if(action == GLFW_RELEASE) {
                    map_entry.second.currentValue = 0.0f;
                    map_entry.second.pressedThisStep = false;
                }
            }
        }
    }
    
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS) {
        MouseClickEvent click(xpos, ypos, button);
        event_handler->emit(click);
    }
}
void InputSystem::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    int windowWidth, windowHeight;
    glfwGetWindowSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float xPercent = xpos / windowWidth;
    float yPercent = ypos / windowHeight;
    //cur [0,1.0]
    
    xPercent = (xPercent * 2.0) - 1.0f;
    yPercent = (yPercent * 2.0) - 1.0f;
    //cur [-1.0, 1.0]
    
    yPercent *= -1.0f;
    //y starts 0 at top
    
    for(auto& map_entry : control_map) {
        if(map_entry.second.type == INPUT_MOUSE_POSITION_X) {
            map_entry.second.currentValue = xPercent;
        }
        if(map_entry.second.type == INPUT_MOUSE_POSITION_Y) {
            map_entry.second.currentValue = yPercent;
        }
    }
}

void InputSystem::resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
