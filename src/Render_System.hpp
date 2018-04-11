//
//  Render_System.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//

#ifndef Render_System_h
#define Render_System_h

#include "System.hpp"

#include <memory> //shared_ptr

#include "EntityManager.hpp"
#include "WindowManager.h" //WindowManager
#include "Program.h"

struct VoxelMeshData
{
    GLuint noOfIndices;
    GLenum indexType;
    GLuint indexBuffer;
    GLuint vertexBuffer;
    GLuint vertexArrayObject;
    glm::vec3 translation;
    float scale;
    double dirty_time;
};

class Render_System : System {
public:
    std::shared_ptr<EntityManager> entity_manager;
    WindowManager* window_manager;
    //std::vector<VoxelMeshData> voxel_meshes;
    std::multimap<Entity_Id, VoxelMeshData> voxel_meshes;
    
    void draw(double t, std::shared_ptr<Program> program);
    void draw_entities(double t, std::shared_ptr<Program> program);
    void draw_voxels(double t, std::shared_ptr<Program> program);
    
    //Voxel helpers
    void calculate_mesh(double t, Entity_Id id, Voxel_Component *voxel_component);
    
    template <typename MeshType>
    void addMesh(double t, Entity_Id id, const MeshType& surfaceMesh, const PolyVox::Vector3DInt32& translation = PolyVox::Vector3DInt32(0, 0, 0), float scale = 1.0f);
    
    void addMeshData(Entity_Id id, VoxelMeshData meshData);
    
    //Camera Functions
    void setMVPE(double t, std::shared_ptr<Program> program);
    void renderGUI();
};

#endif /* Render_System_h */
