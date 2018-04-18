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

void InputSystem::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
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
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if(button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        MouseClickEvent click(xpos, ypos, GLFW_MOUSE_BUTTON_1);
        event_handler->emit(click);
    }
}
void InputSystem::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{}

void InputSystem::resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
