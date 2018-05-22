//
//  PlayerSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/20/18.
//

#include "PlayerSystem.hpp"
#include "InputSystem.hpp"

#include "GLMUtils.hpp"

using namespace glm;

void PlayerSystem::init(const std::string& resourceDirectory) {
    player_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(player_id);
    Camera_Component* camera = entity_manager->add_component<Camera_Component>(player_id);
    Physics_Component* physics = entity_manager->add_component<Physics_Component>(player_id);
    
    position->position = vec3(30.0f, 120.0f, 0.0f);
    position->rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    
    btCollisionShape* fallShape = new btBoxShape( btVector3(0.25, 0.8, 0.25) );
    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(1, 0, 0, 0), btVector3(30, 120, 0)));
    btScalar mass = 5.0f;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
    fallRigidBody->setDamping(0.2, 0.5);
    fallRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    
    bullet_dynamics_world->addRigidBody(fallRigidBody);
    fallRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    physics->body = fallRigidBody;
    player_body = fallRigidBody;
}


void PlayerSystem::step(double t, double dt) {
    //printf("Active? %d\n", player_body->getActivationState());
    //player_body->activate();
    switch (player_state) {
        case FPS_MODE:
            fps_step(t, dt);
            break;
        case RTS_MODE:
            rts_step(t, dt);
            break;
        default:
            std::cerr << "Unexpected Switch Case in PlayerSystem.\n";
            break;
    }
}

void PlayerSystem::fps_step(double t, double dt) {
    if(t < 4.0f) {
        btTransform lock_trans;
        lock_trans.setIdentity();
        lock_trans.setOrigin(btVector3(30.0f, 120.0f, 30.0f));
        
        player_body->setCenterOfMassTransform(lock_trans);
        player_body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
    } else {
        Position_Component* position = entity_manager->get_component<Position_Component>(player_id);
        
        if(input_system->isControlDownThisStep("key_space")) {
            if(abs(player_body->getLinearVelocity().getY()) < 0.01f) {
                player_body->applyCentralImpulse(btVector3(0.0, dt * (550.0f / player_body->getInvMass()), 0.0));
            }
        }
        
        vec3 forwardMove = position->rotation * vec3(0.0f, 0.0f, 1.0f);
        forwardMove = vec3(forwardMove.x, 0.0f, forwardMove.z);
        
        vec3 rightwardMove = cross(forwardMove, vec3(0.0f, 1.0f, 0.0f));
        
        float force_scalar = 50.0f;
        float delta_angle = glm::radians(5.0f);
        
        if(input_system->isControlDownThisStep("key_w")) {
            player_body->applyCentralImpulse( dt * force_scalar * btVector3(forwardMove.x, forwardMove.y, forwardMove.z));
        }
        if(input_system->isControlDownThisStep("key_s")) {
            player_body->applyCentralImpulse(-dt * force_scalar * btVector3(forwardMove.x, forwardMove.y, forwardMove.z));
        }
        if(input_system->isControlDownThisStep("key_d")) {
            player_body->applyCentralImpulse( dt * force_scalar * btVector3(rightwardMove.x, rightwardMove.y, rightwardMove.z));
        }
        if(input_system->isControlDownThisStep("key_a")) {
            player_body->applyCentralImpulse(-dt * force_scalar * btVector3(rightwardMove.x, rightwardMove.y, rightwardMove.z));
        }
        
        vec3 relative_x_axis = position->rotation * (vec3(1.0f, 0.0f, 0.0f));
        
        if(input_system->isControlDownThisStep("key_up")) {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(-1.0f * delta_angle, relative_x_axis);
            position->rotation = deltaRotation * position->rotation;
            
        }
        if(input_system->isControlDownThisStep("key_down")) {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(1.0f * delta_angle, relative_x_axis);
            position->rotation = deltaRotation * position->rotation;
        }
        
        if(input_system->isControlDownThisStep("key_left")) {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(1.0f * delta_angle, glm::vec3(0.0f, 1.0f, 0.0f));
            position->rotation = deltaRotation * position->rotation;
        }
        if(input_system->isControlDownThisStep("key_right"))
        {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(-1.0f * delta_angle, normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
            position->rotation = deltaRotation * position->rotation;
        }
        
        if(input_system->isControlDownThisStep("mouse_right")) {
            float scroll_degree_ratio = 5.0f;
            
            float deltaX = input_system->getCurrentControlValue("mouse_x") - input_system->getPreviousControlValue("mouse_x");
            float deltaY = input_system->getCurrentControlValue("mouse_y") - input_system->getPreviousControlValue("mouse_y");
            float x_radians = radians(scroll_degree_ratio * deltaX);
            float y_radians = radians(scroll_degree_ratio * deltaY);
            
            vec3 relative_x_axis = position->rotation * (vec3(1.0f, 0.0f, 0.0f));
            vec3 relative_y_axis = (vec3(0.0f, 1.0f, 0.0f));
            
            glm::quat deltaRotationX = glm::angleAxis(1.0f * x_radians, relative_y_axis);
            glm::quat deltaRotationY = glm::angleAxis(1.0f * -y_radians, relative_x_axis);
            
            position->rotation = deltaRotationY * deltaRotationX * position->rotation;
            glfwSetInputMode(window_manager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window_manager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void PlayerSystem::rts_step(double t, double dt) {
    switch(selection_state) {
        case BUILD_MODE:
            break;
        case COLONIST_MODE:
            break;
        default:
            std::cerr << "Unexpected Switch Case in PlayerSystem.\n";
            break;
    }
}
