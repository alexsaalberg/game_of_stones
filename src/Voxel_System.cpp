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

using namespace std;
using namespace glm;

//Voxel Stuff
void Voxel_System::draw_voxels(float t, std::shared_ptr<Program> program) {
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    program->bind();
    
    setMVPE(program);
    
    vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
    
    Voxel_Component* voxel_component;
    
    poly_vox_example.render(program);
    
    program->unbind();
}


void Voxel_System::setMVPE(std::shared_ptr<Program> program) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float aspect = windowWidth/(float)windowHeight;
    Camera::aspect = aspect;
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    Camera::setModelIdentityMatrix(program);
    Camera::setViewMatrix(camera, position, program);
    Camera::setProjectionMatrix(program);
    Camera::setEyePosition(position->position, program);
    Camera::setLight(program);
}

void Voxel_System::receive(const MouseClickEvent& collision) {
    vector<Entity_Id> voxel_list = entity_manager->get_ids_with_component<Voxel_Component>();
    
    Voxel_Component* voxel_component;
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* camera_position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    vec3 position = camera_position->position;
    
    if(camera == NULL) {
        printf("Trying to do picking without camera\n");
        exit(-1);
    }
    
    mat4 V = Camera::getViewMatrix(camera, camera_position);
    mat4 P = Camera::getProjectionMatrix();
    
    mat4 invV = inverse( V );
    mat4 invP = inverse( P );
    
    vec4 start(collision.x, collision.y, 0.01f, 1.0f);
    
    printf("1(%f %f %f %f)\n", start.x, start.y, start.z, start.w);
    
    start = invV * invP * start;
    start = start / start.w;
    
    printf("2(%f %f %f %f)\n", start.x, start.y, start.z, start.w);
    
    //Vector3DFloat start(position.x, position.y, position.z);
    //Vector3DFloat end(position.x, position.y, position.z);
    
    for(Entity_Id id : voxel_list) {
        voxel_component = entity_manager->get_component<Voxel_Component>(id);
        
        
    }
    
    printf("Received click at x%lf y%lf!\n", collision.x, collision.y);
}
