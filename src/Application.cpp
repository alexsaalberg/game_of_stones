//
//  Application.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include <btBoxShape.h>
#include <btSphereShape.h>

#include "Application.hpp"
#include "stb_image.h"

using namespace std;
using namespace glm;
//using namespace PolyVox;

void Application::init(double t, const std::string& resourceDirectory) {
	currentState = make_shared<State>();
	previousState = make_shared<State>();
    
    initBullet();
    
    entity_manager = make_shared<EntityManager>();
    event_handler = make_shared<EventHandler>();
    
    input_system.entity_manager = entity_manager;
    input_system.event_handler = event_handler;
    
    render_system.entity_manager = entity_manager;
    
    chunk_system.entity_manager = entity_manager;
    chunk_system.input_system = &input_system;
    chunk_system.bullet_dynamics_world = bullet_dynamics_world;
    
    pick_system.input_system = &input_system;
    pick_system.entity_manager = entity_manager;
    pick_system.chunk_system = &chunk_system;
    
    input_system.addKeyControl("key_x", GLFW_KEY_X);
    input_system.addKeyControl("key_b", GLFW_KEY_B);
    input_system.addKeyControl("key_p", GLFW_KEY_P);
    input_system.addKeyControl("key_v", GLFW_KEY_V);
    input_system.addKeyControl("key_y", GLFW_KEY_Y);
    input_system.addKeyControl("key_g", GLFW_KEY_G);
    input_system.addKeyControl("key_c", GLFW_KEY_C);
    
    input_system.addKeyControl("key_w", GLFW_KEY_W);
    input_system.addKeyControl("key_a", GLFW_KEY_A);
    input_system.addKeyControl("key_s", GLFW_KEY_S);
    input_system.addKeyControl("key_d", GLFW_KEY_D);
    
    input_system.addKeyControl("key_i", GLFW_KEY_I);
    input_system.addKeyControl("key_q", GLFW_KEY_Q);
    
    
    input_system.addKeyControl("key_space", GLFW_KEY_SPACE);
    input_system.addMouseclickControl("mouse_left", GLFW_MOUSE_BUTTON_LEFT);
    input_system.addMouseclickControl("mouse_right", GLFW_MOUSE_BUTTON_RIGHT);
    input_system.addMouseposXControl("mouse_x");
    input_system.addMouseposYControl("mouse_y");
    
    volume_render_system.entity_manager = entity_manager;
    selection_system.entity_manager = entity_manager;
    
    pick_system.selection_id = -1;
    
    //render_system.initVoxels();
    
	initShaders(resourceDirectory+"/shaders");
    initTextures(resourceDirectory+"/models");
    initGeom(resourceDirectory+"/models");
    
    initCamera();
    initPlayer();
    initVoxels();
    
    
    pick_system.cursor_id = player_id;
    
    /*
    vec3 base_pos = vec3(10.0f, 0.0f, 0.0f);
    vec3 pos = vec3(0.0f);
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            for(int k = 0; k < 10; k++) {
                pos = vec3(i, j, k);
                initHelicopter(base_pos + pos);
            }
        }
    }*/
    //event_handler->subscribe<MouseClickEvent>(&voxel_system);
    event_handler->subscribe<MouseClickEvent>(&chunk_system);
    //event_handler->subscribe<MouseClickEvent>(static_cast<Receiver<MouseClickEvent>*>(&voxel_system));
    
    
    chunk_system.addLoader(t, camera_id);
}

void Application::initBullet() {
    // Build the broadphase
    broadphase = new btDbvtBroadphase();
    
    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;
    
    // The world.
    bullet_dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    
    bullet_dynamics_world->setGravity(btVector3(0, -16, 0));
    
    //Init camera
    btCollisionShape* fallShape = new btBoxShape( btVector3(0.25, 0.8, 0.25) );
    //btCollisionShape* fallShape = new btSphereShape(0.4f);
    btDefaultMotionState* fallMotionState =
    new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
    btScalar mass = 5.0f;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
    fallRigidBody->setDamping(0.2, 0.5);
    fallRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    bullet_dynamics_world->addRigidBody(fallRigidBody);
    camera_body = fallRigidBody;
    camera_motion_state = fallMotionState;
    
    //delete fallMotionState;
    //delete fallShape;
    
    //Init debug draw
    bullet_draw = new BulletDraw();
    bullet_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    bullet_draw->setDebugMode(1);

    bullet_dynamics_world->setDebugDrawer(bullet_draw);
}

void Application::initShaders(const std::string& resourceDirectory)
{
    /*
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
     */
    GLSL::checkVersion();
    glClearColor(.9f, .9f, 1.0f, 1.0f); // Set background color.
    glEnable(GL_DEPTH_TEST); // Enable z-buffer test.
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    initMainProgram(resourceDirectory);
    initSimpleProgram(resourceDirectory);
    initVoxelProgram(resourceDirectory);
}

void Application::initMainProgram(const std::string& resourceDirectory) {
    // Initialize the GLSL program.
    mainProgram = make_shared<Program>();
    mainProgram->setVerbose(true);
    mainProgram->setShaderNames(resourceDirectory + "/mainVert.glsl",
                                resourceDirectory + "/mainFrag.glsl");
    
    if (! mainProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    mainProgram->addUniform("P");
    mainProgram->addUniform("V");
    mainProgram->addUniform("trans_inv_V");
    mainProgram->addUniform("M");
    mainProgram->addUniform("mAmbientCoefficient");
    mainProgram->addUniform("mDiffusionCoefficient");
    mainProgram->addUniform("mSpecularCoefficient");
    mainProgram->addUniform("mSpecularAlpha");
    mainProgram->addUniform("eyePosition");
    mainProgram->addUniform("directionTowardsLight");
    //mainProgram->addUniform("Texture0");
    mainProgram->addAttribute("vPosition");
    mainProgram->addAttribute("vNormal");
    mainProgram->addAttribute("vTextureCoordinates");
}


void Application::initSimpleProgram(const std::string& resourceDirectory) {
    // Initialize the GLSL program.
    simpleProgram = make_shared<Program>();
    simpleProgram->setVerbose(true);
    simpleProgram->setShaderNames(resourceDirectory + "/simple_vert.glsl",
                                resourceDirectory + "/simple_frag.glsl");
    
    if (! simpleProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    simpleProgram->addUniform("P");
    simpleProgram->addUniform("V");
    simpleProgram->addAttribute("vPosition");
}

void Application::initVoxelProgram(const std::string& resourceDirectory) {
    voxelProgram = make_shared<Program>();
    voxelProgram->setVerbose(true);
    voxelProgram->setShaderNames(resourceDirectory + "/voxel_normalCalc_vert.glsl",
                                  resourceDirectory + "/voxel_normalCalc_frag.glsl");
    
    if (! voxelProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    //Transformation Matrices
    voxelProgram->addUniform("P");
    voxelProgram->addUniform("V");
    voxelProgram->addUniform("trans_inv_V");
    voxelProgram->addUniform("M");
    //Material constants
    voxelProgram->addUniform("mAmbientCoefficient");
    voxelProgram->addUniform("mDiffusionCoefficient");
    voxelProgram->addUniform("mSpecularCoefficient");
    voxelProgram->addUniform("mSpecularAlpha");
    //Lighting
    voxelProgram->addUniform("eyePosition");
    voxelProgram->addUniform("directionTowardsLight");
    
    voxelProgram->addAttribute("vPosition");
}

void Application::initTextures(const std::string& resourceDirectory) {
    
}

void Application::initGeom(const std::string& resourceDirectory) {
    // this is the tiny obj shapes - not to be confused with our shapes
    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    
    string errStr;
    
    //load in the mesh and make the shapes
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               (resourceDirectory + "/sphere.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        sphereModel = make_shared<Model>();
        sphereModel->createModel(TOshapes, objMaterials);
    }
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                          (resourceDirectory + "/Helicopter.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        helicopterModel = make_shared<Model>();
        helicopterModel->createModel(TOshapes, objMaterials);
        helicopterModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        helicopterModel->scale *= 2.0f;
        printf("Helicopter Model!\n");
    }
    
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                            (resourceDirectory + "/cube.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        cubeModel = make_shared<Model>();
        cubeModel->createModel(TOshapes, objMaterials);
        cubeModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        cubeModel->scale *= 0.51f;
        printf("Cube Model!\n");
    }
}

void Application::initCamera() {
    //camera = make_shared<Camera>();
    camera_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(camera_id);
    Camera_Component* camera = entity_manager->add_component<Camera_Component>(camera_id);
    camera->distance = 500.0f;
    
    position->position = vec3(500.0f, 120.0f, 500.0f);
    position->rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    
    
    glm::quat deltaRotation;
    deltaRotation = glm::angleAxis(1.0f * 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    position->rotation *= deltaRotation;
    
}

void Application::initPlayer() {
    player_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(player_id);
    Model_Component* renderable = entity_manager->add_component<Model_Component>(player_id);
    
    position->position = vec3(1.0f);
    renderable->model = cubeModel;
}

void Application::initVoxels() {
    voxel_id = entity_manager->create_entity();
    
    PagedVolume_Component* voxels = entity_manager->add_component<PagedVolume_Component>(voxel_id);
    voxels->dirty_time = -1.0f;
}

void Application::initHelicopter(glm::vec3 position) {
    int id = entity_manager->create_entity();
    Position_Component* pos = entity_manager->add_component<Position_Component>(id);
    Model_Component* renderable = entity_manager->add_component<Model_Component>(id);
    
    pos->position = position;
    renderable->model = helicopterModel;
}

void Application::integrate(double t, float dt) {
    btScalar timestep = dt;
    //printf("timestep: %f\n", timestep);
    bullet_dynamics_world->stepSimulation(timestep, 10);
    
    input_system.step(t, dt);
	//previousState = make_shared<State>( *currentState );
    currentState->integrate(t, dt);
    pick_system.step(t, dt);
    chunk_system.step(t, dt);
    selection_system.step(t, dt);
    
    camera_body->activate();
    
    if(input_system.isControlDownThisStep("key_space")) {
        if(abs(camera_body->getLinearVelocity().getY()) < 0.01f) {
            camera_body->applyCentralImpulse(btVector3(0.0, dt * (550.0f / camera_body->getInvMass()), 0.0));
        }
    }
    
    btTransform trans;
    camera_body->getMotionState()->getWorldTransform(trans);
    Position_Component *camera_position_component = entity_manager->get_component<Position_Component>(camera_id);

    if(t < 8.0f) {
        btTransform motion;
        motion.setIdentity();
        
        trans.setOrigin(btVector3(camera_position_component->position.x, 120.0f, camera_position_component->position.z));
        camera_motion_state->setWorldTransform(motion);
        camera_body->setWorldTransform(trans);
    }
    camera_position_component->position.x = trans.getOrigin().getX()+0.25f;
    camera_position_component->position.y = trans.getOrigin().getY()+1.6f;
    camera_position_component->position.z = trans.getOrigin().getZ()+0.25f;;
    
    if(input_system.isControlDownThisStep("key_p")) {
        printf("Pos: "); printVec3(camera_position_component->position); printf("\n");
    }
    
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Entity_Id camera_id = camera_ids.at(0);
    
    Position_Component* position = entity_manager->get_component<Position_Component>(camera_id);
    
    vec3 forwardMove = position->rotation * vec3(0.0f, 0.0f, 1.0f);
    forwardMove = vec3(forwardMove.x, 0.0f, forwardMove.z);
    
    vec3 rightwardMove = cross(forwardMove, vec3(0.0f, 1.0f, 0.0f));
    
    float force_scalar = 50.0f;
    if(input_system.isControlDownThisStep("key_w")) {
        camera_body->applyCentralImpulse( dt * force_scalar * btVector3(forwardMove.x, forwardMove.y, forwardMove.z));
    }
    if(input_system.isControlDownThisStep("key_s")) {
        camera_body->applyCentralImpulse(-dt * force_scalar * btVector3(forwardMove.x, forwardMove.y, forwardMove.z));
    }
    if(input_system.isControlDownThisStep("key_d")) {
        camera_body->applyCentralImpulse( dt * force_scalar * btVector3(rightwardMove.x, rightwardMove.y, rightwardMove.z));
    }
    if(input_system.isControlDownThisStep("key_a")) {
        camera_body->applyCentralImpulse(-dt * force_scalar * btVector3(rightwardMove.x, rightwardMove.y, rightwardMove.z));
    }
    
    if(input_system.isControlDownThisStep("key_y")) {
        trans.setOrigin(btVector3(trans.getOrigin() + btVector3(0.0f, 5.0f, 0.0f)));
        camera_body->setWorldTransform(trans);
        
        printf("getting unstuck!\n");
    }
    
    if(input_system.isControlDownThisStep("mouse_right")) {
        float scroll_degree_ratio = 5.0f;
        
        float deltaX = input_system.getCurrentControlValue("mouse_x") - input_system.getPreviousControlValue("mouse_x");
        float deltaY = input_system.getCurrentControlValue("mouse_y") - input_system.getPreviousControlValue("mouse_y");
        float x_radians = radians(scroll_degree_ratio * deltaX);
        float y_radians = radians(scroll_degree_ratio * deltaY);
        vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
        Entity_Id camera_id = camera_ids.at(0);
        
        //Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_id);
        Position_Component* position = entity_manager->get_component<Position_Component>(camera_id);
        
        vec3 relative_x_axis = position->rotation * (vec3(1.0f, 0.0f, 0.0f));
        vec3 relative_y_axis = (vec3(0.0f, 1.0f, 0.0f));
        
        glm::quat deltaRotationX = glm::angleAxis(1.0f * x_radians, relative_y_axis);
        glm::quat deltaRotationY = glm::angleAxis(1.0f * -y_radians, relative_x_axis);
        
        position->rotation = deltaRotationY * deltaRotationX * position->rotation;
        glfwSetInputMode(window_manager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window_manager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
}

void Application::render(double t, float alpha) {
    renderState(*currentState.get(), t);
}

void Application::renderState(State& state, double t) {
    CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    CHECKED_GL_CALL( glDisable(GL_CULL_FACE) ) ; //default, two-sided rendering
    CHECKED_GL_CALL( glCullFace(GL_BACK) );
    
    if( !input_system.isControlDownThisStep("key_i") ) {
        
        render_system.render(t, mainProgram);
        //selection_system.render(t, mainProgram);
        
        //render_system.draw_voxels(t, voxelProgram);
        //chunk_system.recalculateAllMeshes();
        chunk_system.renderAllChunks(t, voxelProgram);
        
        volume_render_system.render(t, voxelProgram);
    } else {
        simpleProgram->bind();
        
        vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
        Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
        Position_Component* position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
        
        Camera::setViewMatrix(camera, position, simpleProgram);
        Camera::setProjectionMatrix(simpleProgram);
        
        bullet_dynamics_world->debugDrawWorld();
        bullet_draw->drawAllLines(simpleProgram);
        simpleProgram->unbind();
    }
    
    //render_system.draw_voxels(entity_manager, t, mainProgram);
}

//[0,1.0]
float Application::randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

//[-1.0, 1.0]
float Application::randomFloatNegativePossible() {
    return (randomFloat() * 2.0f) - 1.0f;
}
