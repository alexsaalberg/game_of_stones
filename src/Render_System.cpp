//
//  Render_System.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/28/18.
//
#include "Render_System.hpp"


#include "GLSL.h" //CHECK_GL_CALL, among others
//imgui
#include "imgui.h"

using namespace std;
using namespace glm;

void Render_System::draw(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float aspect = windowWidth/(float)windowHeight;
    shared_ptr<MatrixStack> M;
    M = make_shared<MatrixStack>();
    
    CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    CHECKED_GL_CALL( glDisable(GL_CULL_FACE) ) ; //default, two-sided rendering
    
    program->bind();
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    
    setModelIdentityMatrix(program);
    setViewMatrix(camera, program);
    setProjectionMatrix(program, aspect);
    
    setEyePosition(camera, program);
    //camera->cameraTheta = t * 50.0f;
    //camera->cameraDistance = 30.0f;// + (t * t );
    
    vec3 directionFromLight = vec3(0.0f) - vec3(-5.0f, 20.0f, 10.0f); //from X to origin
    vec3 directionTowardsLight = -directionFromLight;
    CHECKED_GL_CALL( glUniform3f(program->getUniform("directionTowardsLight"), directionTowardsLight.x, directionTowardsLight.y, directionTowardsLight.z) );
    
    setMaterial(program, 6);
    
    
    draw_entities(entity_manager, t, program);
    
    program->unbind();
}

void Render_System::draw_entities(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    vector<Entity_Id> id_list = entity_manager->get_ids_with_components<Position_Component, Renderable_Component>();
    
    Renderable_Component* renderable_component;
    Position_Component* position_component;
    
    for(Entity_Id id : id_list) {
        renderable_component = entity_manager->get_component<Renderable_Component>(id);
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
}

//Camera Stuff
void Render_System::setModelIdentityMatrix(shared_ptr<Program> program) {
    auto M = make_shared<MatrixStack>();
    M->pushMatrix();
    M->loadIdentity();
    CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())) );
    M->popMatrix();
}

std::shared_ptr<MatrixStack> Render_System::getViewMatrix(Camera_Component* camera) {
    float x = cos(radians(camera->phi))*cos(radians(camera->theta));
    float y = sin(radians(camera->phi));
    float z = cos(radians(camera->phi))*sin(radians(camera->theta));
    
    vec3 identityVector = vec3(0.0f) - vec3(x, y, z); //from origin to xyz
    identityVector *= camera->distance;
    
    std::shared_ptr<MatrixStack> V = make_shared<MatrixStack>();
    V->pushMatrix();
    V->loadIdentity();
    V->lookAt(vec3(0, 0, 0), identityVector, vec3(0, 1, 0));
    V->translate((-1.0f * vec3(0.0f, 0.0f, 0.0f))); //Negative
    V->translate(identityVector);
    return V;
}

void Render_System::setViewMatrix(Camera_Component* camera, std::shared_ptr<Program> program) {
    std::shared_ptr<MatrixStack> V = getViewMatrix(camera);
    CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()) ) );
}

std::shared_ptr<MatrixStack> Render_System::getProjectionMatrix(float aspect) {
    std::shared_ptr<MatrixStack> P = make_shared<MatrixStack>();
    P->pushMatrix();
    P->perspective(45.0f, aspect, 0.1f, 1000.0f);
    return P;
}

void Render_System::setProjectionMatrix(shared_ptr<Program> program, float aspect) {
    shared_ptr<MatrixStack> P = getProjectionMatrix(aspect);
    CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())) );
}

void Render_System::setEyePosition(Camera_Component* camera, shared_ptr<Program> prog) {
    CHECKED_GL_CALL( glUniform3f(prog->getUniform("eyePosition"), camera->distance, 0.0f, 0.f) );
}

void Render_System::renderGUI() {
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

void Render_System::setMaterial(const std::shared_ptr<Program> prog, int i)
{
    CHECKED_GL_CALL( glUniform3f(prog->getUniform("mSpecularCoefficient"), 0.3f, 0.2f, 0.1f) );
    CHECKED_GL_CALL( glUniform1f(prog->getUniform("mSpecularAlpha"), 3.0f) );
    
    switch (i)
    {
        case 0: //shiny blue plastic
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.02f, 0.04f, 0.2f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.0f, 0.16f, 0.9f);;
            break;
        case 1: // flat grey
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.13f, 0.13f, 0.14f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.3f, 0.3f, 0.4f);
            break;
        case 2: //brass
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.3294f, 0.2235f, 0.02745f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7804f, 0.5686f, 0.11373f);
            break;
        case 3: //copper
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.1913f, 0.0735f, 0.0225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
            break;
        case 4: //green man
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.735f, 0.0225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.038f, 0.048f, 0.028f);
            break;
        case 5: //radiation
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.7, 0.7735f, 0.225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
            break;
        case 6: //stone
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.1735f, 0.1225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.438f, 0.4048f, 0.428f);
            break;
        case 7:
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.17f, 0.01f, 0.01f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.61f, 0.04f, 0.04f);
            break;
    }
}
