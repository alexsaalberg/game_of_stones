//
//  System.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#include <vector>

#include "System.hpp"

#include "MatrixStack.h"

using namespace std;

void Render_System::draw(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    vector<Entity_Id> id_list = entity_manager->get_ids_with_components<Position_Component, Renderable_Component>();
    
    Renderable_Component* renderable_component;
    Position_Component* position_component;
    
    for(Entity_Id id : id_list) {
        renderable_component = entity_manager->get_component<Renderable_Component>(id);
        position_component = entity_manager->get_component<Position_Component>(id);
        
        auto M = make_shared<MatrixStack>();
        
        M->pushMatrix();
        M->loadIdentity();
        M->translate(position_component->position);
        //M->scale(gameObject.scale);
        //M->rotate(gameObject.rotation.x, vec3(1, 0, 0));
        //M->rotate(gameObject.rotation.y, vec3(0, 1, 0));
        //M->rotate(gameObject.rotation.z, vec3(0, 0, 1));
        
        renderable_component->model->draw(program, M);
        
        M->popMatrix();
    }
}
