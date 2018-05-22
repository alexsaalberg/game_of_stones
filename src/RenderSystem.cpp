//
//  RenderSystem.cpp
//  CastleSim
//
//  file created by Alex Saalberg on 3/28/18.
//
#include "RenderSystem.hpp"

#include "GLSL.hpp" //CHECK_GL_CALL, among others

#include <btBulletDynamicsCommon.h>

#include "FastNoise.h"
#include "NoisePager.h"

#include "Camera.hpp"

//value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> //quat stuff

using namespace std;
using namespace glm;
using namespace PolyVox;

//Virtual Overloads
void RenderSystem::init(const std::string& resourceDirectory) {
    // Initialize the GLSL program.
    program = make_shared<Program>();
    program->setVerbose(true);
    program->setShaderNames(resourceDirectory + "/shaders/mainVert.glsl",
                                resourceDirectory + "/shaders/mainFrag.glsl");
    
    if (! program->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    program->addUniform("P");
    program->addUniform("V");
    program->addUniform("trans_inv_V");
    program->addUniform("M");
    program->addUniform("mAmbientCoefficient");
    program->addUniform("mDiffusionCoefficient");
    program->addUniform("mSpecularCoefficient");
    program->addUniform("mSpecularAlpha");
    program->addUniform("eyePosition");
    program->addUniform("directionTowardsLight");
    //program->addUniform("Texture0");
    program->addAttribute("vPosition");
    program->addAttribute("vNormal");
    program->addAttribute("vTextureCoordinates");
}

void RenderSystem::render(double t, std::shared_ptr<Program> program) {
    this->program->bind();
        setMVPE(t, this->program);
        draw_entities(t, this->program);
    this->program->unbind();
}

//Other functions
void RenderSystem::draw_entities(double t, std::shared_ptr<Program> program) {
    Camera::setMaterial(program, 6);
    
    vector<EntityId> id_list = entity_manager->get_ids_with_components<Position_Component, Model_Component>();
    
    Model_Component* renderable_component;
    Position_Component* position_component;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glLineWidth( 16.0f );
    
    for(EntityId id : id_list) {
        renderable_component = entity_manager->get_component<Model_Component>(id);
        position_component = entity_manager->get_component<Position_Component>(id);
        
        auto M = make_shared<MatrixStack>();
        
        M->pushMatrix();
        M->loadIdentity();
        M->translate(position_component->position);
        //M->scale(gameObject.scale);
        //M->rotate(gameObject.rotation.x, vec3(1, 0, 0));
        //M->rotate(gameObject.rotation.y, vec3(0, 1, 0));
        //M->rotate(gameObject.rotation.z, vec3(0, 0, 1));
        
        renderable_component->model->draw(program, M);
        
        M->popMatrix();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderSystem::setMVPE(double t, std::shared_ptr<Program> program) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float aspect = windowWidth/(float)windowHeight;
    Camera::aspect = aspect;
    
    vector<EntityId> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    Camera::setModelIdentityMatrix(program);
    Camera::setViewMatrix(camera, position, program);
    Camera::setProjectionMatrix(program);
    Camera::setEyePosition(position->position, program);
    Camera::setLight(program);
}

/*
void RenderSystem::renderGUI() {
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
*/

