//
//  VoxelSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/7/18.
//

#include <vector>

#include "VoxelSystem.hpp"
#include "Component.hpp"
#include "Camera.h"

#include "PolyVox/Picking.h"

using namespace std;
using namespace glm;
using namespace PolyVox;

void VoxelSystem::update(double t) {
    
    for(MouseClickEvent& click : click_events) {
        processClickEvent(t, click);
    }
    click_events.clear();
}

void VoxelSystem::processClickEvent(double t, MouseClickEvent& click) {

    vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
    Voxel_Component* voxel_component;
    
    voxel_component = entity_manager->get_component<Voxel_Component>(voxel_list.at(0));
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* camera_position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    if(camera == NULL) {
        printf("Trying to do picking without camera. Very odd.\n");
        exit(-1);
    }
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    mat4 V = Camera::getViewMatrix(camera, camera_position);
    mat4 P = Camera::getProjectionMatrix();
    mat4 PV = P * V;
    
    mat4 invPV = inverse( PV );
    
    
    float xpercent = click.x / windowWidth;
    float ypercent = click.y / windowHeight;
    //cur [0,1.0]
    
    xpercent = (xpercent * 2.0) - 1.0f;
    ypercent = (ypercent * 2.0) - 1.0f;
    //cur [-1.0, 1.0]
    
    ypercent *= -1.0f;
    //y starts 0 at top
    
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
    
    if(result.didHit) {
        const int radius = 3;
        for(int x = -radius; x < radius; x++) {
            for(int y = -radius; y < radius; y++) {
                for(int z = -radius; z < radius; z++) {
                    locToSet = Vector3DInt32(x + result.hitVoxel.getX(), y + result.hitVoxel.getY(), z + result.hitVoxel.getZ());
                    voxel_component->volume->setVoxel(locToSet, 255-128);
                }
            }
        }
        
        voxel_component->dirty_time = t;
        //printf("%f: Set Dirty Time\n", voxel_component->dirty_time);
    }
}

void VoxelSystem::receive(const MouseClickEvent& collision) {
    click_events.push_back(collision);
}
