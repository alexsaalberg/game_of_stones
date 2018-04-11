//
//  Voxel_System.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/7/18.
//

#include <vector>

#include "Voxel_System.hpp"
#include "Component.hpp"
#include "Camera.h"

#include "PolyVox/Picking.h"

using namespace std;
using namespace glm;
using namespace PolyVox;

void Voxel_System::receive(const MouseClickEvent& collision) {
    vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
    Voxel_Component* voxel_component;
    
    voxel_component = entity_manager->get_component<Voxel_Component>(voxel_list.at(0));
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* camera_position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    if(camera == NULL) {
        printf("Trying to do picking without camera\n");
        exit(-1);
    }
    
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    mat4 V = Camera::getViewMatrix(camera, camera_position);
    mat4 P = Camera::getProjectionMatrix();
    mat4 PV = P * V;
    
    mat4 invV = inverse( V );
    mat4 invP = inverse( P );
    mat4 invPV = inverse( PV );
    
    float xpercent = collision.x / windowWidth;
    float ypercent = collision.y / windowHeight;
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
    
    printf("Camera at (%f %f %f)\n", camera_position->position.x, camera_position->position.y, camera_position->position.z);
    
    const uint8_t emptyVoxelExample = 0; //A voxel value of zero will represent empty space.
    
    const float length = 300.0f;
    vec4 direction = normalize(end - start);
    vec4 directionAndLength = direction * length;
    
    printf("direction %f %f %f %f\n", direction.x, direction.y, direction.z, direction.w);

    Vector3DFloat rayStart(start.x, start.y, start.z);
    Vector3DFloat rayDirectionAndLength(directionAndLength.x, directionAndLength.y, directionAndLength.z);
    
    PickResult result = pickVoxel(voxel_component->volume.get(), rayStart, rayDirectionAndLength, emptyVoxelExample);
    
    printf("Hit? %d\n", result.didHit);
    
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
        printf("Setting radius %d around %d %d %d\n", radius, result.hitVoxel.getX(), result.hitVoxel.getY(), result.hitVoxel.getZ());
        
        voxel_component->dirty_time = glfwGetTime();
    }
    
    //camera_position->position = vec3( locToSet.getX(), locToSet.getY(), locToSet.getZ() - 10 );
    
    
    //Vector3DFloat start(position.x, position.y, position.z);
    //Vector3DFloat end(position.x, position.y, position.z);
    
                                  
    for(Entity_Id id : voxel_list) {
        voxel_component = entity_manager->get_component<Voxel_Component>(id);
    }
    
    printf("Received click at x%lf y%lf!\n", collision.x, collision.y);
}
