//
//  PlayerGraphicsComponent.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/2/18.
//

#include "PlayerGraphicsComponent.hpp"

using namespace std;
using namespace glm;

void PlayerGraphicsComponent::update(GameObject& gameObject, float t, std::shared_ptr<Program> prog) {
    draw(gameObject, t, prog);
}

void PlayerGraphicsComponent::draw(GameObject& gameObject, float t, const std::shared_ptr<Program> prog) {
    auto M = make_shared<MatrixStack>();
    
    
    
    M->pushMatrix();
    M->loadIdentity();
    M->translate(gameObject.position);
        M->scale(gameObject.scale);
            M->rotate(gameObject.rotation.x, vec3(1, 0, 0));
            M->rotate(gameObject.rotation.y, vec3(0, 1, 0));
            M->rotate(gameObject.rotation.z, vec3(0, 0, 1));
    
    M->pushMatrix();
        M->translate(vec3(0.9, 0.9f, 0.0f));
        M->rotate(-t * 10.0f, vec3(0, 1, 0));
        M->translate(vec3(-0.05, 0.0f, 0.45));
        Application::SetMaterial(prog, blade_material);
        this->models.at(1)->draw(prog, M);
    M->popMatrix();
    
    //base
    Application::SetMaterial(prog, windshield_material);
    this->models.at(0)->draw(prog, M, 0);
    
    Application::SetMaterial(prog, blade_material);
    this->models.at(0)->draw(prog, M, 1);
    
    Application::SetMaterial(prog, blade_material);
    this->models.at(0)->draw(prog, M, 2);
    
    
    /*
    for(int i = 0; i < 3; i++) {
        
        
        this.DefaultGraphicsComponent->models.at(i);
        
    }*/
    
    M->popMatrix();
}
