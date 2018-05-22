//
//  Application.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include <btBoxShape.h>
#include <btSphereShape.h>

#include "Application.hpp"

using namespace std;
using namespace glm;
//using namespace PolyVox;

void Application::init(double t, const std::string& resourceDirectory) {
    //Bullet
    initBullet();
    
    //Systems
    event_handler = make_shared<EventHandler>();
    
    entity_manager = new EntityManager();
    
    System::entity_manager = entity_manager;
    System::input_system = &input_system;
    System::bullet_dynamics_world = bullet_dynamics_world;
    System::window_manager = window_manager;
    
    system_manager.add(&chunk_system);
    system_manager.add(&input_system);
    system_manager.add(&volume_render_system);
    system_manager.add(&pick_system);
    system_manager.add(&player_system);
    system_manager.add(&render_system);
    system_manager.add(&selection_system);
    system_manager.add(&physics_system);
    
    system_manager.init(resourceDirectory);
    
    input_system.event_handler = event_handler;
    
    //chunk_system.input_system = &input_system;
    
    //pick_system.input_system = &input_system;
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
    
    input_system.addKeyControl("key_up", GLFW_KEY_UP);
    input_system.addKeyControl("key_down", GLFW_KEY_DOWN);
    input_system.addKeyControl("key_left", GLFW_KEY_LEFT);
    input_system.addKeyControl("key_right", GLFW_KEY_RIGHT);
    
    input_system.addKeyControl("key_i", GLFW_KEY_I);
    input_system.addKeyControl("key_q", GLFW_KEY_Q);
    
    
    input_system.addKeyControl("key_space", GLFW_KEY_SPACE);
    input_system.addMouseclickControl("mouse_left", GLFW_MOUSE_BUTTON_LEFT);
    input_system.addMouseclickControl("mouse_right", GLFW_MOUSE_BUTTON_RIGHT);
    input_system.addMouseposXControl("mouse_x");
    input_system.addMouseposYControl("mouse_y");
    
    //render_system.initVoxels();
    
	initShaders(resourceDirectory+"/shaders");
    initGeom(resourceDirectory+"/models");

    initVoxels();

    event_handler->subscribe<MouseClickEvent>(&chunk_system);
    
    //chunk_system.addLoader(t, player_id);
    chunk_system.addLoader(t, player_system.player_id);
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
    initSimpleProgram(resourceDirectory);
    initVoxelProgram(resourceDirectory);
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
    }
}

void Application::initVoxels() {
    voxel_id = entity_manager->create_entity();
    
    PagedVolume_Component* voxels = entity_manager->add_component<PagedVolume_Component>(voxel_id);
    voxels->dirty_time = -1.0f;
}

void Application::integrate(double t, float dt) {
    // Do Physics
    btScalar timestep = dt;
    bullet_dynamics_world->stepSimulation(timestep, 10);
    
    // Step the Systems
    system_manager.step(t, dt);
}

void Application::render(double t, float alpha) {
    CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    CHECKED_GL_CALL( glCullFace(GL_BACK) ); //Cull back faces
    
    if( !input_system.isControlDownThisStep("key_i") ) {
        // Normal Draw
        render_system.render(t, mainProgram); //Renders models
        chunk_system.renderAllChunks(t, voxelProgram); //Renders world
        volume_render_system.render(t, voxelProgram); //Renders non-world volumes
    } else {
        // If 'i' is held down, debug draw
        simpleProgram->bind();
            vector<EntityId> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
            Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
            Position_Component* position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
        
            Camera::setViewMatrix(camera, position, simpleProgram);
            Camera::setProjectionMatrix(simpleProgram);
        
            bullet_dynamics_world->debugDrawWorld();
            bullet_draw->drawAllLines(simpleProgram);
        simpleProgram->unbind();
    }
}

//[0,1.0]
float Application::randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

//[-1.0, 1.0]
float Application::randomFloatNegativePossible() {
    return (randomFloat() * 2.0f) - 1.0f;
}
