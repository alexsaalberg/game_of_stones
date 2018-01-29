//
//  GameObject.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "GameObject.hpp"

using namespace std;

shared_ptr<GameObject> interpolate(shared_ptr<GameObject> previous, shared_ptr<GameObject> current, float alpha)
{
    shared_ptr<GameObject> gameObject;
    gameObject = make_shared<GameObject>();
    
    gameObject->position = current->position*alpha + previous->position*(1-alpha);
    gameObject->velocity = current->velocity*alpha + previous->velocity*(1-alpha);
    
    return gameObject;
}

GameObject::GameObject(InputComponent* inputComponent,
                       PhysicsComponent* physicsComponent, GraphicsComponent* graphicsComponent) {
    this->input = inputComponent;
    this->physics = physicsComponent;
    this->graphics = graphicsComponent;
}

void GameObject::simulate(float dt)
{
    input->update(*this);
    physics->update(*this, dt);
}

void GameObject::render(std::shared_ptr<Program> prog) {
    graphics->update(*this, prog);
}
