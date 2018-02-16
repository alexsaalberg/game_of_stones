//
//  GameObject.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include <stdio.h>
#include <memory> //std::shared_ptr

#include "glm/glm.hpp"

#include "Program.h"
#include "Component.hpp"

#include "Box2D/Box2D.h"

class InputComponent;
class PhysicsComponent;
class GraphicsComponent;

class GameObject
{
public:
    const glm::vec3 forwardDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    
    //Variables
    b2Body *body;
    
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 omega = glm::vec3(0.0f); //rotational velocity
    
    glm::vec3 impulse = glm::vec3(0.0f);
    
    float scale = 1.0f;
    float radius = 1.0f;
    float mass = 1.0f; //1 gram?
    float collisionCooldown = 0.0f;
    
    int health = 30;
    
    //Functions
    static std::shared_ptr<GameObject> interpolate(std::shared_ptr<GameObject> previous, std::shared_ptr<GameObject> current, float alpha);
    
    GameObject(const GameObject &gameObject); //Copy constructor
    GameObject(std::shared_ptr<InputComponent> input, std::shared_ptr<PhysicsComponent> physics, std::shared_ptr<GraphicsComponent> graphics);
    
    void interpolate(float t, float alpha);
    
    void simulate(float dt);
    void render(std::shared_ptr<Program> prog);
    
    std::shared_ptr<InputComponent> input;
    std::shared_ptr<PhysicsComponent> physics;
    std::shared_ptr<GraphicsComponent> graphics;
};

#endif /* GameObject_hpp */
