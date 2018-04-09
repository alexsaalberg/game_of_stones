//
//  Render_System.cpp
//  CastleSim
//
//  file created by Alex Saalberg on 3/28/18.
//
#include "Render_System.hpp"


#include "GLSL.h" //CHECK_GL_CALL, among others
//imgui
#include "imgui.h"

#include "FastNoise.h"
#include "NoisePager.h"

#include "Camera.h"

//value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> //quat stuff

using namespace std;
using namespace glm;
using namespace PolyVox;

void Render_System::initVoxels() {
    //volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,63,63,63));
    //createSphereInVolume(*volData.get(), 30.0f);
    
    volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,127,127,127));
    pagedData = make_shared<PagedVolume<uint8_t>>(new NoisePager());
    
    //createLand(*volData.get());
    //createFunction3D(*volData.get());
    
    //voxel_rend.initCubicMesh_RawVolume(volData.get());
    Region region = Region(1,1,1,244,244,244);
    //Region region = Region(1,1,1,255,255,255);
    //Region region2 = Region(-100,-100,-100,100,100,100);
    
    //pagedData->flushAll();
    //pagedData->prefetch(region2);
    //clearRegion(*pagedData.get(), region2);
    //createLand(*pagedData.get(), region);
    
    
    const int32_t extractedRegionSize = 64;
    int meshCounter = 0;
    const int32_t render_edge_length = 256;
    const int32_t render_height = 64;
    
    
    for (int32_t z = -render_edge_length / 2; z < render_edge_length /2; z += extractedRegionSize)
    {
        for (int32_t y = 0; y < render_height; y += extractedRegionSize)
        {
            for (int32_t x = -render_edge_length / 2; x < render_edge_length / 2; x += extractedRegionSize)
            {
                // Specify the region to extract based on a starting position and the desired region sze.
                PolyVox::Region regToExtract(x, y, z, x + extractedRegionSize, y + extractedRegionSize, z + extractedRegionSize);
                
                // If you uncomment this line you will be able to see that the volume is rendered as multiple seperate meshes.
                //regToExtract.shrink(1);
                
                // Perform the extraction for this region of the volume
                auto mesh = extractCubicMesh(pagedData.get(), regToExtract);
                //auto mesh = extractMarchingCubesMesh(pagedData.get(), regToExtract);
                
                // The returned mesh needs to be decoded to be appropriate for GPU rendering.
                auto decodedMesh = decodeMesh(mesh);
                
                // Pass the surface to the OpenGL window. Note that we are also passing an offset in this multi-mesh example. This is because
                // the surface extractors return a mesh with 'local space' positions to reduce storage requirements and precision problems.
                poly_vox_example.addMesh(decodedMesh, decodedMesh.getOffset());
                
                meshCounter++;
                //printf("Mesh #%d\n", meshCounter);
            }
        }
    }
    
    
    //voxel_rend.initCubicMesh_RawVolume(volData.get());
    //voxel_rend.initCubicMesh_PagedVolume(pagedData.get(), region);
    
    // Perform the extraction for this region of the volume
    //auto mesh = extractCubicMesh(pagedData.get(), region);
    
    // The returned mesh needs to be decoded to be appropriate for GPU rendering.
    //auto decodedMesh = decodeMesh(mesh);
    
    //poly_vox_example.addMesh(decodedMesh);
}

void Render_System::draw(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    program->bind();
    
    setMVPE(entity_manager, t, program);
    
    draw_entities(entity_manager, t, program);
    
    program->unbind();
}

void Render_System::draw_entities(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    setMaterial(program, 6);
    
    vector<Entity_Id> id_list = entity_manager->get_ids_with_components<Position_Component, Model_Component>();
    
    Model_Component* renderable_component;
    Position_Component* position_component;
    
    for(Entity_Id id : id_list) {
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
}

void Render_System::setMVPE(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float aspect = windowWidth/(float)windowHeight;
    Camera::aspect = aspect;
    
    
    vector<Entity_Id> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    setModelIdentityMatrix(program);
    setViewMatrix(camera, position, program);
    setProjectionMatrix(program);
    
    setEyePosition(position->position, program);
    
    vec3 directionFromLight = vec3(0.0f) - vec3(-5.0f, 200.0f, 10.0f); //from X to origin
    vec3 directionTowardsLight = -directionFromLight;
    CHECKED_GL_CALL( glUniform3f(program->getUniform("directionTowardsLight"), directionTowardsLight.x, directionTowardsLight.y, directionTowardsLight.z) );
}

//Voxel Stuff
void Render_System::draw_voxels(shared_ptr<EntityManager> entity_manager, float t, std::shared_ptr<Program> program) {
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    program->bind();
    
    setMVPE(entity_manager, t, program);
    
    setMaterial(program, 1);
    poly_vox_example.render(program);
    
    program->unbind();
}


void Render_System::createFunction3D(RawVolume<uint8_t>& volData)
{
    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth()-3; z++)
    {
        for (int x = 0; x < volData.getWidth()-3; x++)
        {
            for (int y = 0; y < volData.getHeight()-3; y++)
            {
                uint8_t uVoxelValue = 0;
                
                float xCenter = volData.getWidth() / 2.0f;
                float zCenter = volData.getDepth() / 2.0f;
                float yCenter = volData.getHeight() / 2.0f;
                
                float xF = (float) x;
                float zF = (float) z;
                float yF = (float) y;
                
                xF -= xCenter;
                zF -= zCenter;
                yF -= yCenter;
                
                float scale = 0.035f;
                xF *= scale;
                zF *= scale;
                yF *= scale;
                
                
                //sin(10(x^2+y^2))/10
                float calc = sin(10.0f * (xF*xF+zF*zF))/10.0f;
                //printf("calc %f\n", calc);
                
                //(0.4^2-(0.6-(x^2+y^2)^0.5)^2)^0.5
                calc = xF*xF + zF*zF;
                calc = 0.6f - pow(calc, 0.5f);
                calc = pow(0.4f, 2.0f) - pow(calc, 2.0f);
                calc = pow(calc, 0.5f);
                
                calc = pow(xF, 2.0f) + (9.0f * pow(yF, 2.0f)) / 4.0f + pow(zF, 2.0f) - 1.0f;
                calc = pow(calc, 3.0f);
                
                calc = calc - pow(xF, 2.0f)*pow(zF, 3.0f);
                calc = calc - (9.0f * pow(yF, 2.0f) * pow(zF, 3.0f) / 80.0f);
                
                
                //calc = (0.4^2 - (0.6f - (xF*xF + zF*zF)^0.5f)^2.0f)^0.5f;
                
                //1/(15(x^2+y^2))
                //calc = 1 / (15.0f*(xF*xF+zF*zF));
                
                //calc = (calc + 1.0f) / 2.0f;
                //float threshold = (calc * 100.0f);
                
                uVoxelValue = 0;
                if(abs(calc) < 0.2f) {
                    uVoxelValue = 5;
                }
                volData.setVoxel(x, y, z, uVoxelValue);
            }
        }
        
    }
    
}

void Render_System::createFunction(RawVolume<uint8_t>& volData)
{
    
    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth()-3; z++)
    {
            for (int x = 0; x < volData.getWidth()-3; x++)
            {
                uint8_t uVoxelValue = 0;
                
                float xCenter = volData.getWidth() / 2.0f;
                float zCenter = volData.getDepth() / 2.0f;
                
                float xF = (float) x;
                float zF = (float) z;
                
                xF -= xCenter;
                zF -= zCenter;
                
                float scale = 0.02f;
                xF *= scale;
                zF *= scale;
                
                
                //sin(10(x^2+y^2))/10
                float calc = sin(10.0f * (xF*xF+zF*zF))/10.0f;
                //printf("calc %f\n", calc);
                
                //(0.4^2-(0.6-(x^2+y^2)^0.5)^2)^0.5
                calc = xF*xF + zF*zF;
                calc = 0.6f - pow(calc, 0.5f);
                calc = pow(0.4f, 2.0f) - pow(calc, 2.0f);
                calc = pow(calc, 0.5f);
                
                
                //calc = (0.4^2 - (0.6f - (xF*xF + zF*zF)^0.5f)^2.0f)^0.5f;
                
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

void Render_System::clearRegion(PagedVolume<uint8_t>& volData, Region region) {
    for(int x = region.getLowerX(); x < region.getUpperX(); x++) {
        for(int z = region.getLowerZ(); z < region.getUpperZ(); z++) {
            for(int y = region.getLowerY(); y < region.getUpperY(); y++) {
                volData.setVoxel(x, y, z, 0);
            }
        }
    }
}

void Render_System::createLand(PagedVolume<uint8_t>& volData, Region region)
{
    FastNoise myNoise; // Create a FastNoise object
    //myNoise.SetSeed(13);
    myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
    
    myNoise.SetFractalOctaves(4);
    myNoise.SetFractalLacunarity(3);
    myNoise.SetFractalGain(0.5f);
    
    //This vector hold the position of the center of the volume
    
    uint8_t voxelVal = 0;
    
    for(int x = region.getLowerX(); x <= region.getUpperX(); x++) {
        for(int z = region.getLowerZ(); z <= region.getUpperZ(); z++) {
            
            float val = myNoise.GetValue(x, z);
            
            val += 1.0f;
            val *= 0.5f;
            
            int height = (int) (val * 100.0f);
            
            //printf("X%d Z%d, height %d\n", x, z, height);
            
            for(int y = region.getLowerY(); y <= region.getUpperY(); y++) {
                voxelVal = 0;
                
                if(y < height) {
                    //printf("X%d Z%d, Y%d H%d\n", x, z, y, height);
                    voxelVal = 255-128;
                }
                
                volData.setVoxel(x, y, z, voxelVal);
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
            
            //printf("X%d Z%d, height %d\n", x, z, height);
            
            for(int y = 0; y <= volData.getHeight() - 1; y++) {
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

std::shared_ptr<MatrixStack> Render_System::getViewMatrix(Camera_Component* camera, Position_Component* camera_position) {
    vec3 norot_identity = vec3(0.0f, 0.0f, 1.0f);
    
    vec3 position = camera_position->position;
    vec3 identity = camera_position->rotation * norot_identity;
    identity = position + identity;
    
    std::shared_ptr<MatrixStack> V = make_shared<MatrixStack>();
    V->pushMatrix();
    
    //V->loadIdentity();
    //V->multMatrix(&rot); //rotate based on camera's rotation
    V->lookAt(position, identity, vec3(0, 1, 0)); //Look at identity from position
    //V->translate(-1.0f * position); //Negative
    return V;
}

void Render_System::setViewMatrix(Camera_Component* camera,  Position_Component* camera_position, std::shared_ptr<Program> program) {
    //std::shared_ptr<MatrixStack> V = Camera::getViewMatrix(camera, camera_position);
    glm::mat4 V = Camera::getViewMatrix(camera, camera_position);
    CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V) ) );
}

std::shared_ptr<MatrixStack> Render_System::getProjectionMatrix(float aspect) {
    
    std::shared_ptr<MatrixStack> P = make_shared<MatrixStack>();
    P->pushMatrix();
    P->perspective(45.0f, aspect, 0.1f, 1000.0f);
    return P;
}

void Render_System::setProjectionMatrix(shared_ptr<Program> program) {
    glm::mat4 P = Camera::getProjectionMatrix();
    CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P)) );
}

void Render_System::setEyePosition(vec3 position, shared_ptr<Program> prog) {

    //float x = cos(radians(camera->phi))*cos(radians(camera->theta));
    //float y = sin(radians(camera->phi));
    //float z = cos(radians(camera->phi))*sin(radians(camera->theta));
    
    //vec3 invertedPosition = vec3(0.0f) - position; //from origin to xyz
    
    CHECKED_GL_CALL( glUniform3fv(prog->getUniform("eyePosition"), 1, value_ptr(position)) );
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
    CHECKED_GL_CALL( glUniform1f(prog->getUniform("mSpecularAlpha"), 1.0f) );
    
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
        case 8:
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.6f, 0.01f, 0.01f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.61f, 0.04f, 0.04f);
            break;
    }
}
