//
//  Voxel_System.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/7/18.
//

#include <vector>

#include "Voxel_System.hpp"
#include "Component.hpp"

using namespace std;

void Voxel_System::receive(const MouseClickEvent& collision) {
    vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
    
    Voxel_Component* voxel_component;
    Camera_Component* camera = entity_manager->get_first_component_of_type<Camera_Component>();
    
    if(camera == NULL) {
        printf("Trying to do picking without camera\n");
        exit(-1);
    }
    
    
    for(Entity_Id id : voxel_list) {
        voxel_component = entity_manager->get_component<Voxel_Component>(id);
        
    }
    
    printf("Received click at x%lf y%lf!\n", collision.x, collision.y);
}
