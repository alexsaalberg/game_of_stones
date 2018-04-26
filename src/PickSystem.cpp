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
    float screenx = input_system->getCurrentControlValue("mouse_x");
    float screeny = input_system->getCurrentControlValue("mouse_y");
    
    bool onTop = true;
    
    if(input_system->isControlDownThisStep("key_x")) {
        onTop = false; //select the existing block, not the air block on top
    }
    
    //Calculate picked point
    Vector3DInt32 point = pickScreen(screenx, screeny, onTop);
    
    //Update cursor position
    Position_Component* position_component = entity_manager->get_component<Position_Component>(cursor_id);
    glm::vec3 new_cursor_position = vec3((float)point.getX(), (float)point.getY(), (float)point.getZ());
    position_component->position = new_cursor_position;
    
    if(input_system->wasControlPressedThisStep("mouse_left")) {
        if(entity_manager->entityExists(selection_id)) { //we're completing a selection region
            
            Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
            
            Region& region = selection_component->selection.region;
            
            region.setUpperCorner(point);
            
            printf("Selection Finished: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
            
        } else { //we're creating a new selection
            Vector3DInt32 point = pickScreen(screenx, screeny, onTop);
            
            selection_id = entity_manager->create_entity();
            Selection_Component* selection_component = entity_manager->add_component<Selection_Component>(selection_id);
            
            Region& region = selection_component->selection.region;
            
            region.setLowerCorner(point);
            region.setUpperCorner(point);
            
            
            printf("Selection Started: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
            
        }
    }
    if(input_system->wasControlPressedThisStep("key_space")) {
        Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
        Region& region = selection_component->selection.region;
        
        printf("Deleting Region: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
        deleteRegion(t, region);
        entity_manager->delete_entity(selection_id);
    }
}

Region PickSystem::createProperRegion(Region &region) {
    Region new_region;
    
    if(region.getLowerX() < region.getUpperX()) {
        new_region.setLowerX(region.getLowerX());
        new_region.setUpperX(region.getUpperX());
    } else {
        new_region.setLowerX(region.getUpperX());
        new_region.setUpperX(region.getLowerX());
    }
    
    if(region.getLowerY() < region.getUpperY()) {
        new_region.setLowerY(region.getLowerY());
        new_region.setUpperY(region.getUpperY());
    } else {
        new_region.setLowerY(region.getUpperY());
        new_region.setUpperY(region.getLowerY());
    }
    
    if(region.getLowerZ() < region.getUpperZ()) {
        new_region.setLowerZ(region.getLowerZ());
        new_region.setUpperZ(region.getUpperZ());
    } else {
        new_region.setLowerZ(region.getUpperZ());
        new_region.setUpperZ(region.getLowerZ());
    }
    
    return new_region;
}

void PickSystem::deleteRegion(double t, Region& region) {
    
    Region region_to_delete = createProperRegion(region);
    printf("Deleting Region: (%d %d %d)(%d %d %d)\n", region_to_delete.getLowerX(), region_to_delete.getLowerY(), region_to_delete.getLowerZ(), region_to_delete.getUpperX(), region_to_delete.getUpperY(), region_to_delete.getUpperZ());
    
    std::vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
    Voxel_Component* voxel_component;
    voxel_component = entity_manager->get_component<Voxel_Component>(voxel_list.at(0));
    
    for(int x = region_to_delete.getLowerX(); x <= region_to_delete.getUpperX(); x++) {
        for(int y = region_to_delete.getLowerY(); y <= region_to_delete.getUpperY(); y++) {
            for(int z = region_to_delete.getLowerZ(); z <= region_to_delete.getUpperZ(); z++) {
                Vector3DInt32 point(x, y, z);
                
                voxel_component->volume->setVoxel(point, 0);
                chunk_system->setDirtyTimeViaVoxel(t, point);
            }
        }
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

