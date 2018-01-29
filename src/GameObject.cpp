//
//  GameObject.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "GameObject.hpp"

GameObject::GameObject(InputComponent* inputComponent,
                       PhysicsComponent* physicsComponent, GraphicsComponent* graphicsComponent) {
    this->input = inputComponent;
    this->physics = physicsComponent;
    this->graphics = graphicsComponent;
}

void GameObject::interpolate(float t, float alpha) {
    
}

void GameObject::simulate(float dt)
{
    input->update(*this);
    physics->update(*this, dt);
}

void GameObject::render(std::shared_ptr<Program> prog) {
    graphics->update(*this, prog);
}
