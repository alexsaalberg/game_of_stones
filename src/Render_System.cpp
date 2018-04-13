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

/*
void Render_System::initVoxels() {
    //volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,63,63,63));
    //createSphereInVolume(*volData.get(), 30.0f);
    
    volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,127,127,127));
    pagedData = make_shared<PagedVolume<uint8_t>>(new NoisePager());
    
    
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
}
 */

void Render_System::draw(double t, std::shared_ptr<Program> program) {
    program->bind();
    
    setMVPE(t, program);
    
    draw_entities(t, program);
    
    program->unbind();
}

void Render_System::draw_entities(double t, std::shared_ptr<Program> program) {
    Camera::setMaterial(program, 6);
    
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

void Render_System::setMVPE(double t, std::shared_ptr<Program> program) {
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

//Voxel Stuff
void Render_System::draw_voxels(double t, std::shared_ptr<Program> program) {
    //Calculate Meshes
    vector<Entity_Id> voxel_id_list = entity_manager->get_ids_with_component<Voxel_Component>();
    
    for(Entity_Id id : voxel_id_list) {
        Voxel_Component* voxel_component = entity_manager->get_component<Voxel_Component>(id);
        
        if(voxel_meshes.count(id) == 0) {
            //create mesh
            calculate_mesh(t, id, voxel_component);
            //printf("%lf: Calculated Initial mesh for id %d\n", t, id);
        } else if (voxel_component->dirty_time > t) {
            printf("No Update %lf < %lf\n", t, voxel_component->dirty_time);
            //The physics goes a bit ahead of rendering
            //So it's possible for the voxels to change AFTER the moment we're rendering
            //In this case, do nothing
        } else if (voxel_component->dirty_time > voxel_meshes.find(id)->second.dirty_time) {
            //recreate mesh
            voxel_meshes.erase(id);
            calculate_mesh(t, id, voxel_component);
            //voxel_meshes.find(id)->second.dirty_time = t;
            //printf("%lf: Recalculating mesh for id %d\n", t, id);
        }
    }
    
    //Render
    program->bind();
    
    setMVPE(t, program);
    Camera::setMaterial(program, 1);
    
    std::shared_ptr<MatrixStack> M = std::make_shared<MatrixStack>();
    M->loadIdentity();
    
    program->bind();
    
    for(auto meshIterator : voxel_meshes) {
        auto meshData = meshIterator.second;
        
        M->pushMatrix();
            M->translate(meshData.translation);
            M->scale(meshData.scale);
            glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        M->popMatrix();
        
        // Bind the vertex array for the current mesh
        glBindVertexArray(meshData.vertexArrayObject);
        // Draw the mesh
        glDrawElements(GL_TRIANGLES, meshData.noOfIndices, meshData.indexType, 0);
        // Unbind the vertex array.
        glBindVertexArray(0);
    }
    
    program->unbind();
}
            
void Render_System::calculate_mesh(double t, Entity_Id id, Voxel_Component *voxel_component) {
    const int32_t extractedRegionSize = 64;
    int meshCounter = 0;
    const int32_t render_edge_length = 256;
    const int32_t render_height = 128;
    
    
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
                auto mesh = extractCubicMesh(voxel_component->volume.get(), regToExtract);
                //auto mesh = extractMarchingCubesMesh(pagedData.get(), regToExtract);
                
                // The returned mesh needs to be decoded to be appropriate for GPU rendering.
                auto decodedMesh = decodeMesh(mesh);
                
                // Pass the surface to the OpenGL window. Note that we are also passing an offset in this multi-mesh example. This is because
                // the surface extractors return a mesh with 'local space' positions to reduce storage requirements and precision problems.
                addMesh(t, id, decodedMesh, decodedMesh.getOffset());
                
                meshCounter++;
                //printf("Mesh #%d\n", meshCounter);
            }
        }
    }
}

// Convert a PolyVox mesh to OpenGL index/vertex buffers. Inlined because it's templatised.
template <typename MeshType>
void Render_System::addMesh(double t, Entity_Id id, const MeshType& surfaceMesh, const PolyVox::Vector3DInt32& translation, float scale)
{
    // This struct holds the OpenGL properties (buffer handles, etc) which will be used
    // to render our mesh. We copy the data from the PolyVox mesh into this structure.
    VoxelMeshData meshData;
    
    // Create the VAO for the mesh
    glGenVertexArrays(1, &(meshData.vertexArrayObject));
    glBindVertexArray(meshData.vertexArrayObject);
    
    // The GL_ARRAY_BUFFER will contain the list of vertex positions
    glGenBuffers(1, &(meshData.vertexBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, surfaceMesh.getNoOfVertices() * sizeof(typename MeshType::VertexType), surfaceMesh.getRawVertexData(), GL_STATIC_DRAW);
    
    // and GL_ELEMENT_ARRAY_BUFFER will contain the indices
    glGenBuffers(1, &(meshData.indexBuffer));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaceMesh.getNoOfIndices() * sizeof(typename MeshType::IndexType), surfaceMesh.getRawIndexData(), GL_STATIC_DRAW);
    
    // Every surface extractor outputs valid positions for the vertices, so tell OpenGL how these are laid out
    glEnableVertexAttribArray(0); // Attrib '0' is the vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, position))); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)
    
    // Some surface extractors also generate normals, so tell OpenGL how these are laid out. If a surface extractor
    // does not generate normals then nonsense values are written into the buffer here and sghould be ignored by the
    // shader. This is mostly just to simplify this example code - in a real application you will know whether your
    // chosen surface extractor generates normals and can skip uploading them if not.
    glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, normal)));
    
    // Finally a surface extractor will probably output additional data. This is highly application dependant. For this example code
    // we're just uploading it as a set of bytes which we can read individually, but real code will want to do something specialised here.
    glEnableVertexAttribArray(2); //We're talking about shader attribute '2'
    GLint size = (std::min)(sizeof(typename MeshType::VertexType::DataType), size_t(4)); // Can't upload more that 4 components (vec4 is GLSL's biggest type)
    glVertexAttribIPointer(2, size, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, data)));
    
    // We're done uploading and can now unbind.
    glBindVertexArray(0);
    
    // A few additional properties can be copied across for use during rendering.
    meshData.noOfIndices = surfaceMesh.getNoOfIndices();
    meshData.translation = glm::vec3(translation.getX(), translation.getY(), translation.getZ());
    meshData.scale = scale;
    
    // Set 16 or 32-bit index buffer size.
    meshData.indexType = sizeof(typename MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    
    meshData.dirty_time = t;
    // Now add the mesh to the list of meshes to render.
    addMeshData(id, meshData);
}


void Render_System::addMeshData(Entity_Id id, VoxelMeshData meshData)
{
    std::pair<Entity_Id, VoxelMeshData> element(id, meshData);
    voxel_meshes.insert(element);
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


