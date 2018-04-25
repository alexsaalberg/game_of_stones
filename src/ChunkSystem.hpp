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

#include <set>

#include "PolyVox/Region.h"
#include "PolyVox/Vector.h"
#include "EntityManager.hpp"
#include "WindowManager.h"
#include "System.hpp"
#include "Program.h"
#include "Camera.h"
#include "Messaging.h"
#include "InputSystem.hpp"

const int32_t Chunk_X_Length = 16; //x
const int32_t Chunk_Y_Length = 128; //z
const int32_t Chunk_Z_Length = 16; //y

struct ChunkMeshData
{
    GLuint number_of_indices;
    GLenum index_type;
    GLuint index_buffer;
    GLuint vertex_buffer;
    GLuint vertex_array_object;
    glm::vec3 translation;
    float scale;
    double clean_time;
};

struct ChunkData {
    PolyVox::Vector3DInt32 coords;
    int refs;
    ChunkMeshData mesh;
    double dirty_time;
};

struct ChunkLoader {
    PolyVox::Vector3DInt32 current_chunk_coord;
    PolyVox::Vector3DInt32 previous_chunk_coord;
};

//These two are necessary because Vector3DInt32 don't have < operators.
struct ChunkCompare {
    bool operator() (const PolyVox::Vector3DInt32& left, const PolyVox::Vector3DInt32& right) const {
        std::hash<PolyVox::Vector3DInt32> hasher;
        return hasher(left) < hasher(right);
    }
};
static bool chunk_comp(const PolyVox::Vector3DInt32& left, const PolyVox::Vector3DInt32& right) {
    std::hash<PolyVox::Vector3DInt32> hasher;
    return hasher(left) < hasher(right);
}
//^^^


class ChunkSystem : System, public Receiver<MouseClickEvent> {
public:
//Variables
    InputSystem* input_system;
    
    int radius = 5;
    std::shared_ptr<EntityManager> entity_manager;
    WindowManager* window_manager;
    //std::set<ChunkLoader> entities;
    std::multimap<Entity_Id, ChunkLoader> chunk_loaders;
    std::map<Vector3DInt32, ChunkData, ChunkCompare> chunks;
    std::vector<MouseClickEvent> click_events;
    //std::vector<Entity_Id> entities;
//Virtual Functions
    virtual void receive(const MouseClickEvent& collision);
    void processClickEvent(double t, MouseClickEvent& click);
    
    void update(double t);
    void renderAllChunks(double t, std::shared_ptr<Program> program);
    
//Function
    static std::set<PolyVox::Vector3DInt32, ChunkCompare> calculateChunkSetAroundCoord(PolyVox::Vector3DInt32 chunk_coord);
    static PolyVox::Vector3DInt32 voxelCoordToChunkCoord(PolyVox::Vector3DInt32 voxel_coord);
    static PolyVox::Vector3DInt32 positionToChunkCoord(glm::vec3 position);
    static PolyVox::Region chunkCoordToRegion(PolyVox::Vector3DInt32 chunk_coord);
    
    void addLoader(double t, Entity_Id entity);
    void removeLoader(Entity_Id entity);
    
    void setDirtyTimeViaVoxel(double t, Vector3DInt32& voxel_coord);
    void setDirtyTimeViaChunk(double t, Vector3DInt32 chunk_coord);
    void recalculateAllMeshes();
private:
    
    void addChunk(double t, Vector3DInt32 chunk, Vector3DInt32 center);
    void removeChunk(Vector3DInt32 chunk);
    
    void eraseMeshData(ChunkMeshData& mesh);
    
    ChunkMeshData calculateMesh(double t, PolyVox::Vector3DInt32 chunk);
    template <typename MeshType>
    ChunkMeshData bindMesh(double t, const MeshType& surfaceMesh,  const PolyVox::Vector3DInt32& translation = PolyVox::Vector3DInt32(0, 0, 0), float scale = 1.0f);
    
};

#endif /* ChunkSystem_hpp */
