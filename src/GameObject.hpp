//
//  GameObject.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include <stdio.h>
#include <memory>

#include "glm/glm.hpp"

#include "Program.h"
#include "Component.hpp"

class InputComponent;
class PhysicsComponent;
class GraphicsComponent;

class GameObject
{
public:
    //Variables
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 omega = glm::vec3(0.0f); //rotational velocity
    
    //Functions
    GameObject(InputComponent *input, PhysicsComponent *physics, GraphicsComponent *graphics);
    
    void interpolate(float t, float alpha);
    
    void simulate(float dt);
    void render(std::shared_ptr<Program> prog);
    
private:
    InputComponent *input;
    PhysicsComponent *physics;
    GraphicsComponent *graphics;
};

#endif /* GameObject_hpp */
