//
//  PlayerGraphicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/2/18.
//

#include "PlayerGraphicsComponent.hpp"

using namespace std;
using namespace glm;

void PlayerGraphicsComponent::update(GameObject& gameObject, std::shared_ptr<Program> prog) {
    draw(gameObject, prog);
}

void PlayerGraphicsComponent::draw(GameObject& gameObject, const std::shared_ptr<Program> prog) {
    auto M = make_shared<MatrixStack>();
    
    M->pushMatrix();
    M->loadIdentity();
    M->translate(gameObject.position);
    M->scale(gameObject.scale);
    M->rotate(radians(gameObject.rotation.x), vec3(1, 0, 0));
    M->rotate(radians(gameObject.rotation.y), vec3(0, 1, 0));
    M->rotate(radians(gameObject.rotation.z), vec3(0, 0, 1));
    
    //base
    Application::SetMaterial(prog, main_material);
    this->models.at(0)->draw(prog, M, 0);
    
    Application::SetMaterial(prog, windshield_material);
    this->models.at(0)->draw(prog, M, 1);
    
    Application::SetMaterial(prog, blade_material);
    this->models.at(0)->draw(prog, M, 2);
    
    /*
    for(int i = 0; i < 3; i++) {
        
        
        this.DefaultGraphicsComponent->models.at(i);
        
    }*/
    
    M->popMatrix();
}
