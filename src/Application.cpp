//
//  Application.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include "Application.hpp"
#include "stb_image.h"

using namespace std;
using namespace glm;
//using namespace PolyVox;

void Application::init(double t, const std::string& resourceDirectory) {
	currentState = make_shared<State>();
	previousState = make_shared<State>();
    
    
    entity_manager = make_shared<EntityManager>();
    event_handler = make_shared<EventHandler>();
    
    input_system.entity_manager = entity_manager;
    input_system.event_handler = event_handler;
    input_system.chunk_system = &chunk_system;
    
    voxel_system.entity_manager = entity_manager;
    
    render_system.entity_manager = entity_manager;
    chunk_system.entity_manager = entity_manager;
    
    
    //render_system.initVoxels();
    
	initShaders(resourceDirectory+"/shaders");
    initTextures(resourceDirectory+"/models");
    initGeom(resourceDirectory+"/models");
    
    initCamera();
    initPlayer();
    initVoxels();
    
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


void Application::initShaders(const std::string& resourceDirectory)
{
    /*
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
     */
    GLSL::checkVersion();
    glClearColor(.5f, .5f, .5f, 1.0f); // Set background color.
    glEnable(GL_DEPTH_TEST); // Enable z-buffer test.
    
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
    renderable->model = helicopterModel;
}

void Application::initVoxels() {
    voxel_id = entity_manager->create_entity();
    
    Voxel_Component* voxels = entity_manager->add_component<Voxel_Component>(voxel_id);
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
	//previousState = make_shared<State>( *currentState );
    currentState->integrate(t, dt);
    voxel_system.update(t);
    chunk_system.update(t);
}

void Application::render(double t, float alpha) {
    renderState(*currentState.get(), t);
}

void Application::renderState(State& state, double t) {
    CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    CHECKED_GL_CALL( glDisable(GL_CULL_FACE) ) ; //default, two-sided rendering
    
    render_system.draw(t, mainProgram);
    
    //render_system.draw_voxels(t, voxelProgram);
    //chunk_system.recalculateAllMeshes();
    chunk_system.renderAllChunks(t, voxelProgram);
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
