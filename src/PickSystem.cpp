//
//  PickSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/24/18.
//

#include "PickSystem.hpp"

#include <vector>

#include <glm/gtc/type_ptr.hpp> //mat4

#include "PolyVox/Picking.h"
#include "Camera.h"

using namespace PolyVox;

void PickSystem::update(double t) {
    if(input_system->wasControlPressedThisStep("mouse_left")) {
        
        float screenx = input_system->getCurrentControlValue("mouse_x");
        float screeny = input_system->getCurrentControlValue("mouse_y");
        
        Vector3DInt32 point = pickScreen(screenx, screeny, false);
        
        std::vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
        Voxel_Component* voxel_component;
        
        voxel_component = entity_manager->get_component<Voxel_Component>(voxel_list.at(0));
        voxel_component->volume->setVoxel(point, 0);
        chunk_system->setDirtyTimeViaVoxel(t, point);
    }
    else if(input_system->wasControlPressedThisStep("mouse_right")) {
        float screenx = input_system->getCurrentControlValue("mouse_x");
        float screeny = input_system->getCurrentControlValue("mouse_y");
        
        Vector3DInt32 point = pickScreen(screenx, screeny, true);
        
        std::vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
        Voxel_Component* voxel_component;
        
        voxel_component = entity_manager->get_component<Voxel_Component>(voxel_list.at(0));
        voxel_component->volume->setVoxel(point, 255-128);
        chunk_system->setDirtyTimeViaVoxel(t, point);
    } else {
        float screenx = input_system->getCurrentControlValue("mouse_x");
        float screeny = input_system->getCurrentControlValue("mouse_y");
        
        Vector3DInt32 point;
        if(input_system->isControlDownThisStep("key_x")) {
             point = pickScreen(screenx, screeny, false);
        } else {
             point = pickScreen(screenx, screeny, true);
        }
        glm::vec3 new_cursor_position = vec3((float)point.getX(), (float)point.getY(), (float)point.getZ());
        //new_cursor_position += 0.5f; //center of cube
        
        Position_Component* position_component = entity_manager->get_component<Position_Component>(cursor_id);
        position_component->position = new_cursor_position;
    }
    
}


Vector3DInt32 PickSystem::pickScreen(float xpercent, float ypercent, bool previous) {
    std::vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
    Voxel_Component* voxel_component;
    
    voxel_component = entity_manager->get_component<Voxel_Component>(voxel_list.at(0));
    
    std::vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* camera_position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    if(camera == NULL) {
        printf("pickScreen: No camera. Very odd.\n");
        exit(-1);
    }
    
    mat4 V = Camera::getViewMatrix(camera, camera_position);
    mat4 P = Camera::getProjectionMatrix();
    mat4 PV = P * V;
    
    mat4 invPV = inverse( PV );
    
    vec4 start(xpercent, ypercent, 0.0f, 1.0f);
    vec4 end(xpercent, ypercent, 0.05f, 1.0f);
    
    start = invPV * start;
    start = start / start.w;
    
    end = invPV * end;
    end = end / end.w;
    
    const uint8_t emptyVoxelExample = 0; //A voxel value of zero will represent empty space.
    
    const float length = 300.0f;
    vec4 direction = normalize(end - start);
    vec4 directionAndLength = direction * length;
    
    
    Vector3DFloat rayStart(start.x, start.y, start.z);
    Vector3DFloat rayDirectionAndLength(directionAndLength.x, directionAndLength.y, directionAndLength.z);
    
    PickResult result = pickVoxel(voxel_component->volume.get(), rayStart, rayDirectionAndLength, emptyVoxelExample);
    
    Vector3DInt32 locToSet;
    
    if(previous && result.hasPreviousVoxel) {
        return result.previousVoxel;
    }
    
    if(result.didHit) {
        return result.hitVoxel;
    }
    printf("pickScreen ERROR: DID NOT HIT\n");
    return result.hitVoxel;
}

