//
//  DefaultGraphicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#include "DefaultGraphicsComponent.hpp"

using namespace std;
using namespace glm;

void DefaultGraphicsComponent::update(GameObject& gameObject, std::shared_ptr<Program> prog) {
    draw(gameObject, prog);
}

void DefaultGraphicsComponent::draw(GameObject& gameObject, const std::shared_ptr<Program> prog) {
    
    auto M = make_shared<MatrixStack>();
    
    M->pushMatrix();
    M->loadIdentity();
        M->translate(gameObject.position);
        //M->scale(gameObject.scale);
        M->rotate(radians(gameObject.rotation.x), vec3(1, 0, 0));
        M->rotate(radians(gameObject.rotation.y), vec3(0, 1, 0));
        M->rotate(radians(gameObject.rotation.z), vec3(0, 0, 1));
    
    for (auto &model : models) // access by reference to avoid copying
    {
        model->draw(prog, M);
    }
    
    M->popMatrix();
}
