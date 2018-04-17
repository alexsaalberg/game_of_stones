//
//  ChunkSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/16/18.
//

#ifndef ChunkSystem_hpp
#define ChunkSystem_hpp

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <unordered_set>

#include "PolyVox/Region.h"
#include "PolyVox/Vector.h"
#include "EntityManager.hpp"
#include "System.hpp"

const int32_t Chunk_X_Length = 16; //x
const int32_t Chunk_Y_Length = 16; //z
const int32_t Chunk_Z_Length = 16; //y

struct ChunkMeshData
{
    GLuint number_indices;
    GLenum index_type;
    GLuint index_buffer;
    GLuint vertex_buffer;
    GLuint vertex_array_object;
    glm::vec3 translation;
    float scale;
    double dirty_time;
};

struct ChunkData {
    PolyVox::Vector3DInt32 coords;
    int refs;
    double dirty_time;
};

struct ChunkLoader {
    Vector3DInt32 current_chunk_coord;
    Vector3DInt32 previous_chunk_coord;
};

class ChunkSystem : System {
public:
//Variables
    std::shared_ptr<EntityManager> entity_manager;
    //std::set<ChunkLoader> entities;
    std::multimap<Entity_Id, ChunkLoader> chunk_loaders;
    //std::vector<Entity_Id> entities;
//Functions
    void update(double t);
    
    static std::unordered_set<PolyVox::Vector3DInt32> calculateChunkSetAroundCoord(Vector3DInt32 chunk_coord);
    static PolyVox::Vector3DInt32 voxelCoordToChunkCoord(PolyVox::Vector3DInt32 voxel_coord);
    static PolyVox::Vector3DInt32 positionToChunkCoord(glm::vec3 position);
    static PolyVox::Region chunkCoordToRegion(PolyVox::Vector3DInt32 chunk_coord);
    
    void addLoader(Entity_Id entity);
    void removeLoader(Entity_Id entity);
private:
    
    
};

#endif /* ChunkSystem_hpp */
