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
using namespace PolyVox;

void Application::init(const std::string& resourceDirectory) {
	currentState = make_shared<State>();
	previousState = make_shared<State>();
    entity_manager = make_shared<EntityManager>();
    
    input_system.entity_manager = entity_manager;
    
	initShaders(resourceDirectory+"/shaders");
    initTextures(resourceDirectory+"/models");
    initGeom(resourceDirectory+"/models");
    
    initCamera();
    initPlayer();
    
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
    
    volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,63,63,63));
    createSphereInVolume(*volData.get(), 30.0f);
    voxel_rend.initCubicMesh(volData.get());
}

void Application::createSphereInVolume(RawVolume<uint8_t>& volData, float fRadius)
{
    //This vector hold the position of the center of the volume
    Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);
    
    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth(); z++)
    {
        for (int y = 0; y < volData.getHeight(); y++)
        {
            for (int x = 0; x < volData.getWidth(); x++)
            {
                //Store our current position as a vector...
                Vector3DFloat v3dCurrentPos(x,y,z);
                //And compute how far the current position is from the center of the volume
                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();
                
                uint8_t uVoxelValue = 0;
                
                //If the current voxel is less than 'radius' units from the center then we make it solid.
                if(fDistToCenter <= fRadius)
                {
                    //Our new voxel value
                    uVoxelValue = 255;
                }
                
                //Wrte the voxel value into the volume
                volData.setVoxel(x, y, z, uVoxelValue);
            }
        }
    }
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
    
    position->position = vec3(1.0f);
}

void Application::initPlayer() {
    player_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(player_id);
    Renderable_Component* renderable = entity_manager->add_component<Renderable_Component>(player_id);
    
    position->position = vec3(1.0f);
    renderable->model = helicopterModel;
}

void Application::initHelicopter(glm::vec3 position) {
    int id = entity_manager->create_entity();
    Position_Component* pos = entity_manager->add_component<Position_Component>(id);
    Renderable_Component* renderable = entity_manager->add_component<Renderable_Component>(id);
    
    pos->position = position;
    renderable->model = helicopterModel;
}

void Application::integrate(float t, float dt) {
	//previousState = make_shared<State>( *currentState );

    currentState->integrate(t, dt);
}

void Application::render(float t, float alpha) {
    renderState(*currentState.get(), t);
}

void Application::renderGUI() {
    bool my_tool_active = true;
    // Create a window called "My First Tool", with a menu bar.
    ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
    
    static float my_color[4] = {0.0f};
    // Edit a color (stored as ~4 floats)
    ImGui::ColorEdit4("Color", my_color);
    
    // Plot some values
    const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
    ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));
    
    // Display contents in a scrolling region
    ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
    ImGui::BeginChild("Scrolling");
    for (int n = 0; n < 50; n++)
        ImGui::Text("%04d: Some text", n);
    ImGui::EndChild();
    ImGui::End();
}

void Application::renderState(State& state, float t) {
    render_system.draw(entity_manager, t, mainProgram);
}

//[0,1.0]
float Application::randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

//[-1.0, 1.0]
float Application::randomFloatNegativePossible() {
    return (randomFloat() * 2.0f) - 1.0f;
}
