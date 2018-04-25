//
//  SelectionStack.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/13/18.
//

#include "Selection.hpp"

#include <algorithm>

using namespace PolyVox;


template <class VoxelType>
VoxelType FillSource<VoxelType>::samplePosition(PolyVox::Vector3DInt32 position) {
    return this->voxel;
}

template <class VoxelType>
VoxelType VolumeSource<VoxelType>::samplePosition(PolyVox::Vector3DInt32 position) {
    return this->volume->getVoxel(position);
}

Vector3DInt32 SimpleSampler::getTextureCoordinates(Region region, Vector3DInt32 position) {
    return position;
}


template <class VoxelType>
VoxelType Selection<VoxelType>::samplePosition(PolyVox::Vector3DInt32 position) {
    PolyVox::Vector3DInt32 textureCoordinates = sampler->getTextureCoordinates(region, position);
    
    return source->samplePosition(textureCoordinates);
}
