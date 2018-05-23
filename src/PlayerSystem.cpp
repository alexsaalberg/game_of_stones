//
//  PlayerSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/20/18.
//
#include "PolyVox/Picking.h"

#include "PlayerSystem.hpp"
#include "InputSystem.hpp"

#include "GLMUtils.hpp"
#include "RegionUtils.hpp"
#include "Camera.hpp"

using namespace glm;

void PlayerSystem::init(const std::string& resourceDirectory) {
    // create Player Entity
    player_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(player_id);
    Camera_Component* camera = entity_manager->add_component<Camera_Component>(player_id);
    Physics_Component* physics = entity_manager->add_component<Physics_Component>(player_id);
    
    position->position = vec3(30.0f, 120.0f, 0.0f);
    position->rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    
    // init player physics body
    btCollisionShape* fallShape = new btBoxShape( btVector3(0.25, 0.8, 0.25) );
    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(1, 0, 0, 0), btVector3(30, 120, 0)));
    btScalar mass = 5.0f;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
    fallRigidBody->setDamping(0.2, 0.5);
    fallRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    
    bullet_dynamics_world->addRigidBody(fallRigidBody);
    fallRigidBody->setActivationState(DISABLE_DEACTIVATION); // always active, important
    
    physics->body = fallRigidBody;
    physics->eye_offset = (vec3(0.0f, 0.8f, 0.0f));
    player_body = fallRigidBody;
    
    // default to FPS_MODE & BUILD_MODE
    player_state = FPS_MODE;
    glfwSetInputMode(window_manager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //Create cursor entity
    initCursor(resourceDirectory);
    
}

void PlayerSystem::initCursor(const std::string& resourceDirectory) {
    // This is the tiny obj shapes - not to be confused with our shapes
    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    
    string errStr;
    
    std::shared_ptr<Model> cubeModel;
    
    //load in the mesh and make the shapes
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               (resourceDirectory + "/models/cube.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        cubeModel = make_shared<Model>();
        cubeModel->createModel(TOshapes, objMaterials);
        cubeModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        cubeModel->scale *= 0.51f;
    }
    
    // Cursor Entity
    cursor_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(cursor_id);
    Model_Component* renderable = entity_manager->add_component<Model_Component>(cursor_id);
    position->position = vec3(0.0f);
    position->rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    renderable->model = cubeModel;
    renderable->draw_as_outline = true;
}

void PlayerSystem::step(double t, double dt) {
    if(getInputSystem()->wasControlPressedThisStep("key_g")) {
        if(player_state == FPS_MODE) {
            player_state = RTS_MODE;
            glfwSetInputMode(window_manager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            player_state = FPS_MODE;
            glfwSetInputMode(window_manager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            getEntityManager()->delete_entity(selection_id);
        }
    }
    
    switch (player_state) {
        case FPS_MODE:
            fpsStep(t, dt);
            break;
        case RTS_MODE:
            rtsStep(t, dt);
            break;
        default:
            std::cerr << "Unexpected Switch Case in PlayerSystem.\n";
            break;
    }
}

void PlayerSystem::fpsStep(double t, double dt) {
    if(t < 4.0f) {
        btTransform lock_trans;
        lock_trans.setIdentity();
        lock_trans.setOrigin(btVector3(0.0f, 120.0f, 0.0f));
        
        player_body->setCenterOfMassTransform(lock_trans);
        player_body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
    } else {
        Position_Component* position = entity_manager->get_component<Position_Component>(player_id);
        
        if(input_system->isControlDownThisStep("key_space")) {
            if(abs(player_body->getLinearVelocity().getY()) < 0.01f) {
                player_body->applyCentralImpulse(btVector3(0.0, dt * (550.0f / player_body->getInvMass()), 0.0));
            }
        }
        
        vec3 forwardMove = position->rotation * vec3(0.0f, 0.0f, 1.0f);
        forwardMove = vec3(forwardMove.x, 0.0f, forwardMove.z);
        
        vec3 rightwardMove = cross(forwardMove, vec3(0.0f, 1.0f, 0.0f));
        
        float force_scalar = 50.0f;
        float delta_angle = glm::radians(5.0f);
        
        if(input_system->isControlDownThisStep("key_w")) {
            player_body->applyCentralImpulse( dt * force_scalar * btVector3(forwardMove.x, forwardMove.y, forwardMove.z));
        }
        if(input_system->isControlDownThisStep("key_s")) {
            player_body->applyCentralImpulse(-dt * force_scalar * btVector3(forwardMove.x, forwardMove.y, forwardMove.z));
        }
        if(input_system->isControlDownThisStep("key_d")) {
            player_body->applyCentralImpulse( dt * force_scalar * btVector3(rightwardMove.x, rightwardMove.y, rightwardMove.z));
        }
        if(input_system->isControlDownThisStep("key_a")) {
            player_body->applyCentralImpulse(-dt * force_scalar * btVector3(rightwardMove.x, rightwardMove.y, rightwardMove.z));
        }
        
        // up/down camera movements are relative to current camera
        // left/right aren't because we never do "roll" camera rotations (which alter the rel y axis)
        vec3 relative_x_axis = position->rotation * (vec3(1.0f, 0.0f, 0.0f));
        vec3 relative_y_axis = (vec3(0.0f, 1.0f, 0.0f));
        
        if(input_system->isControlDownThisStep("key_up")) {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(-1.0f * delta_angle, relative_x_axis);
            position->rotation = deltaRotation * position->rotation;
            
        }
        if(input_system->isControlDownThisStep("key_down")) {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(1.0f * delta_angle, relative_x_axis);
            position->rotation = deltaRotation * position->rotation;
        }
        
        if(input_system->isControlDownThisStep("key_left")) {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(1.0f * delta_angle, glm::vec3(0.0f, 1.0f, 0.0f));
            position->rotation = deltaRotation * position->rotation;
        }
        if(input_system->isControlDownThisStep("key_right"))
        {
            glm::quat deltaRotation;
            deltaRotation = glm::angleAxis(-1.0f * delta_angle, normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
            position->rotation = deltaRotation * position->rotation;
        }
        
        //Move camera
        float scroll_degree_ratio = 8.0f;
        
        float deltaX = input_system->getCurrentControlValue("mouse_x") - input_system->getPreviousControlValue("mouse_x");
        float deltaY = input_system->getCurrentControlValue("mouse_y") - input_system->getPreviousControlValue("mouse_y");
        float x_radians = radians(scroll_degree_ratio * deltaX);
        float y_radians = radians(scroll_degree_ratio * deltaY);
        
        glm::quat deltaRotationX = glm::angleAxis(1.0f * -x_radians, relative_y_axis);
        glm::quat deltaRotationY = glm::angleAxis(1.0f * -y_radians, relative_x_axis);
        
        position->rotation = deltaRotationY * deltaRotationX * position->rotation;
        
        // todo: Limit Camera Y rotations
        
        
        //Block Breaking
        Vector3DInt32 break_block_point = polyVoxPickScreen(0.0f, 0.0f, false); //exact middle of screen
        Vector3DInt32 place_block_point = polyVoxPickScreen(0.0f, 0.0f, true); //exact middle of screen
        Region break_region = PolyVox::Region(break_block_point, break_block_point);
        Region place_region = PolyVox::Region(place_block_point, place_block_point);
        
        Position_Component* cursor_position = entity_manager->get_component<Position_Component>(cursor_id);
        glm::vec3 new_cursor_position = vec3((float)break_block_point.getX(), (float)break_block_point.getY(), (float)break_block_point.getZ());
        cursor_position->position = new_cursor_position;
        
        if(getInputSystem()->wasControlPressedThisStep("mouse_left")) {
            fillRegion(t, break_region, BLOCK_AIR);
        } else if (getInputSystem()->wasControlPressedThisStep("mouse_right")) {
            fillRegion(t, place_region, selected_block);
        }
    }
}

void PlayerSystem::rtsStep(double t, double dt) {
    if(getInputSystem()->wasControlPressedThisStep("key_t")) {
        selection_state = (selection_state == BUILD_MODE) ? COLONIST_MODE : BUILD_MODE;
    }
    float screenx = input_system->getCurrentControlValue("mouse_x");
    float screeny = input_system->getCurrentControlValue("mouse_y");
    
    bool onTop = true;
    Vector3DInt32 point = polyVoxPickScreen(screenx, screeny, onTop);
    
    Position_Component* cursor_position = entity_manager->get_component<Position_Component>(cursor_id);
    glm::vec3 new_cursor_position = vec3((float)point.getX(), (float)point.getY(), (float)point.getZ());
    cursor_position->position = new_cursor_position;
    
    if(getInputSystem()->wasControlPressedThisStep("mouse_left")) {
        entity_manager->delete_entity(selection_id);
        
        initial_screenx = screenx;
        initial_screeny = screeny;
        isDrag = false;
    }
    
    if(getInputSystem()->isControlDownThisStep("mouse_left")) {
        mouse_left_downtime += dt;
        
        glm::vec2 cur_screen_coords = glm::vec2(screenx, screeny);
        glm::vec2 init_screen_coords = glm::vec2(initial_screenx, initial_screeny);
        float drag_distance = glm::distance(cur_screen_coords, init_screen_coords);
        
        if(!isDrag && drag_distance > min_drag_length) {
            printf("Drag threshold reached\n");
            isDrag = true;
        }
        
        if(isDrag) {
            
            if(!entity_manager->entityExists(selection_id))  { // we're creating a new selection
                selection_id = entity_manager->create_entity();
                Selection_Component* selection_component = entity_manager->add_component<Selection_Component>(selection_id);
                
                Region& region = selection_component->selection.region;
                
                region.setLowerCorner(point);
                region.setUpperCorner(point);
                
                selection_component->dirty_time = t;
                
                //printf("Selection Started: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
                
            } else { // we're completing a selection region
                Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
                
                Region& region = selection_component->selection.region;
                
                //todo: make sure region is not larger than maximum
                
                region.setUpperCorner(point);
                
                selection_component->dirty_time = t;
            }
        }
    }
        
    if(getInputSystem()->wasControlReleasedThisStep("mouse_left")) {
        if(!isDrag) { // this is a click
            printf("*click*\n");
        }
    }
    
    switch(selection_state) {
        case BUILD_MODE:
            break;
        case COLONIST_MODE:
            break;
        default:
            std::cerr << "Unexpected Switch Case in PlayerSystem.\n";
            break;
    }
}


Vector3DInt32 PlayerSystem::polyVoxPickScreen(float xpercent, float ypercent, bool previous) {
    std::vector<EntityId> voxel_list = entity_manager->get_ids_with_component<PagedVolume_Component>();
    PagedVolume_Component* voxel_component = entity_manager->get_component<PagedVolume_Component>(voxel_list.at(0));
    
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(player_id);
    Position_Component* camera_position = entity_manager->get_component<Position_Component>(player_id);
    
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
    cerr << "PlayerSystem: Screen pick did not hit a voxel.\n";
    return result.hitVoxel;
}

void PlayerSystem::fillRegion(double t, Region& region, CASTLE_VOXELTYPE voxel_type) {
    Region region_to_delete = PolyVoxExtensions::createProperRegion(region);
    printf("Deleting Region: (%d %d %d)(%d %d %d)\n", region_to_delete.getLowerX(), region_to_delete.getLowerY(), region_to_delete.getLowerZ(), region_to_delete.getUpperX(), region_to_delete.getUpperY(), region_to_delete.getUpperZ());
    
    std::vector<EntityId> voxel_list = entity_manager->get_ids_with_component<PagedVolume_Component>();
    PagedVolume_Component* voxel_component;
    voxel_component = entity_manager->get_component<PagedVolume_Component>(voxel_list.at(0));
    
    for(int x = region_to_delete.getLowerX(); x <= region_to_delete.getUpperX(); x++) {
        for(int y = region_to_delete.getLowerY(); y <= region_to_delete.getUpperY(); y++) {
            for(int z = region_to_delete.getLowerZ(); z <= region_to_delete.getUpperZ(); z++) {
                Vector3DInt32 point(x, y, z);
                
                voxel_component->volume->setVoxel(point, voxel_type);
                chunk_system->setDirtyTimeViaVoxel(t, point);
            }
        }
    }
}
