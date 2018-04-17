//
//  ChunkSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/16/18.
//

#include "ChunkSystem.hpp"


using namespace PolyVox;

//Public
void ChunkSystem::update(double t) {
    for(auto chunk_loader_pair : chunk_loaders) {
        Entity_Id id = chunk_loader_pair.first;
        ChunkLoader &loader = chunk_loader_pair.second;
        
        Position_Component* position_component = entity_manager->get_component<Position_Component>(id);
        
        loader.previous_chunk_coord = loader.current_chunk_coord;
        loader.current_chunk_coord = positionToChunkCoord(position_component->position);
        
        if(loader.previous_chunk_coord != loader.current_chunk_coord) {
            //Update references
        }
    }
}

std::unordered_set<PolyVox::Vector3DInt32> calculateChunkSetAroundCoord(Vector3DInt32 chunk_coord) {
    const float radius = 6.0f;
    
    std::unordered_set<Vector3DInt32> return_chunks;
    std::unordered_set<Vector3DInt32> new_chunks;
    
    //return_chunks.insert(chunk_coord);
    new_chunks.insert(chunk_coord);
    
    std::unordered_set<Vector3DInt32> delta_chunks;
    delta_chunks.insert(Vector3DInt32( 1, 0, 0));
    delta_chunks.insert(Vector3DInt32(-1, 0, 0));
    delta_chunks.insert(Vector3DInt32( 0, 1, 0));
    delta_chunks.insert(Vector3DInt32( 0,-1, 0));
    delta_chunks.insert(Vector3DInt32( 0, 0, 1));
    delta_chunks.insert(Vector3DInt32( 0, 0,-1));
    
    for(auto chunk : new_chunks) {
        for(auto delta_chunk : delta_chunks) {
            Vector3DInt32 adjacent_chunk = chunk_coord + delta_chunk;
            if(return_chunks.count(adjacent_chunk) == 0) {
                Vector3DInt32 difference_vec = chunk_coord - adjacent_chunk;
            
                if(difference_vec.length() < radius) {
                    return_chunks.insert(adjacent_chunk);
                    new_chunks.insert(adjacent_chunk);
                }
            }
        }
        new_chunks.erase(chunk);
    }
    
    return return_chunks;
}

void ChunkSystem::addLoader(Entity_Id entity) {
    ChunkLoader chunk_loader;
    chunk_loader.current_chunk_coord = NULL;
    chunk_loader.previous_chunk_coord = NULL;
    //todo: init references
    
    std::pair<Entity_Id, ChunkLoader> element(entity, chunk_loader);
    chunk_loaders.insert(element);
}


void ChunkSystem::removeLoader(Entity_Id entity) {
    chunk_loaders.erase(entity);
    //Remove references
}

//Static
Vector3DInt32 ChunkSystem::voxelCoordToChunkCoord(Vector3DInt32 voxel_coord) {
    //assuming uniform 16 lengths
    //(10, 23, 37) -> (0, 1, 2)
    //10/16 = 0
    //23/16 = 1
    //37/16 = 2
    
    Vector3DInt32 chunk_coord;
    chunk_coord.setX(voxel_coord.getX() / Chunk_X_Length);
    chunk_coord.setY(voxel_coord.getY() / Chunk_Y_Length);
    chunk_coord.setZ(voxel_coord.getZ() / Chunk_Z_Length);
    
    return chunk_coord;
}

Vector3DInt32 ChunkSystem::positionToChunkCoord(glm::vec3 position) {
    int32_t x = floor(position.x + 0.5f);
    int32_t y = floor(position.y + 0.5f);
    int32_t z = floor(position.z + 0.5f);
    
    Vector3DInt32 chunk_coord;
    chunk_coord.setX(x / Chunk_X_Length);
    chunk_coord.setY(y / Chunk_Y_Length);
    chunk_coord.setZ(z / Chunk_Z_Length);
    
    return chunk_coord;
}

Region ChunkSystem::chunkCoordToRegion(Vector3DInt32 chunk_coord) {
    Region region;
    region.setLowerX(chunk_coord.getX());
    region.setLowerY(chunk_coord.getY());
    region.setLowerZ(chunk_coord.getZ());
    
    region.setUpperX(chunk_coord.getX() + Chunk_X_Length - 1); //0 + 16 - 1 = 15
    region.setUpperY(chunk_coord.getY() + Chunk_Y_Length - 1);
    region.setUpperZ(chunk_coord.getZ() + Chunk_Z_Length - 1);
    
    return region;
}
