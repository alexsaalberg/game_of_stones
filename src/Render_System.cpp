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

#include "FastNoise.h"
#include "PolyVox/FilePager.h"

using namespace std;
using namespace glm;
using namespace PolyVox;

void Render_System::initVoxels() {
    //volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,63,63,63));
    //createSphereInVolume(*volData.get(), 30.0f);
    
    volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,127,127,127));
    pagedData = make_shared<PagedVolume<uint8_t>>(new FilePager<uint8_t>());
    
    createLand(*volData.get());
    
    voxel_rend.initCubicMesh(volData.get());
}

void Render_System::draw(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    
    
    
    program->bind();
    
    setMVPE(entity_manager, t, program);
    
    draw_entities(entity_manager, t, program);
    
    program->unbind();
}

void Render_System::draw_entities(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    
    
    setMaterial(program, 6);
    
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

void Render_System::setMVPE(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float aspect = windowWidth/(float)windowHeight;
    
    
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    
    setModelIdentityMatrix(program);
    setViewMatrix(camera, program);
    setProjectionMatrix(program, aspect);
    
    setEyePosition(camera, program);
    //camera->cameraTheta = t * 50.0f;
    //camera->cameraDistance = 30.0f;// + (t * t );
    
    vec3 directionFromLight = vec3(0.0f) - vec3(-5.0f, 0.0f, 10.0f); //from X to origin
    vec3 directionTowardsLight = -directionFromLight;
    CHECKED_GL_CALL( glUniform3f(program->getUniform("directionTowardsLight"), directionTowardsLight.x, directionTowardsLight.y, directionTowardsLight.z) );
}

//Voxel Stuff
void Render_System::draw_voxels(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    program->bind();
    
    setMVPE(entity_manager, t, program);
    
    float radius = 5.0f;
    
    auto M = make_shared<MatrixStack>();
    M->loadIdentity();
    M->translate(radius * vec3(sin(t), 0.0f, cos(t)));
    
    CHECKED_GL_CALL(glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())));

    setMaterial(program, 6);
    voxel_rend.render(program);
    
    program->unbind();
}

void Render_System::createFunction(RawVolume<uint8_t>& volData)
{
    
    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth()-3; z++)
    {
            for (int x = 0; x < volData.getWidth()-3; x++)
            {
                uint8_t uVoxelValue = 0;
                
                float xF = (float) x;
                float zF = (float) z;
                float scale = 0.01f;
                xF *= scale;
                zF *= scale;
                
                //sin(10(x^2+y^2))/10
                float calc = sin(10.0f * (xF*xF+zF*zF))/10.0f;
                //printf("calc %f\n", calc);
                
                //1/(15(x^2+y^2))
                //calc = 1 / (15.0f*(xF*xF+zF*zF));
                
                calc = (calc + 1.0f) / 2.0f;
                float height = (int)(calc * 100.0f);
                
                for (int y = 0; y < volData.getHeight()-3; y++)
                {
                    uVoxelValue = 0;
                    if(y < height) {
                        uVoxelValue = 5;
                    }
                    volData.setVoxel(x, y, z, uVoxelValue);
                }
            }
        
    }
}

void Render_System::createLand(RawVolume<uint8_t>& volData)
{
    FastNoise myNoise; // Create a FastNoise object
    //myNoise.SetSeed(13);
    myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
    
    myNoise.SetFractalOctaves(4);
    myNoise.SetFractalLacunarity(3);
    myNoise.SetFractalGain(0.5f);
    
    //This vector hold the position of the center of the volume
    Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);
    
    /*
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
                //float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();
                float val = myNoise.GetValue(x, y, z);
                if(val > 0.01f || ((x==0) && (y==0) && (z==0))) {
                    //printf("X%d Y%d Z%d =  %f\n", x,y,z, val);
                }
                uint8_t uVoxelValue = 0;
                if(val > 0.5f)
                    uVoxelValue = 1;
                
                //= myNoise.GetValue(x, y);
                
                //Wrte the voxel value into the volume
                volData.setVoxel(x, y, z, uVoxelValue);
            }
        }
    }
     */
    
    for(int x = 0; x < volData.getWidth()-2; x++) {
        for(int z = 0; z < volData.getDepth()-2; z++) {
            float xF = (float) x;
            float zF = (float) z;
            uint8_t voxelVal = 0;
            
            float scale = 0.01f;
            xF *= scale;
            zF *= scale;
            
            float val = myNoise.GetValue(x, z);
            
            val += 1.0f;
            val *= 0.5f;
            
            int height = (int) (val * 100.0f);
            
            printf("X%d Z%d, height %d\n", x, z, height);
            
            for(int y = 0; y < volData.getHeight()-2; y++) {
                voxelVal = 0;
                
                if(y < height)
                    voxelVal = 255;
                
                volData.setVoxel(x, y, z, voxelVal);
            }
        }
    }
}

void Render_System::createSphereInVolume(RawVolume<uint8_t>& volData, float fRadius)
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
    float x = cos(radians(camera->phi))*cos(radians(camera->theta));
    float y = sin(radians(camera->phi));
    float z = cos(radians(camera->phi))*sin(radians(camera->theta));
    
    vec3 cV = vec3(0.0f) - vec3(x, y, z); //from origin to xyz
    cV *= camera->distance;
    
    CHECKED_GL_CALL( glUniform3f(prog->getUniform("eyePosition"), cV.x, cV.y, cV.z) );
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
