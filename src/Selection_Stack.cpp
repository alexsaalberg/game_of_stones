//
//  SelectionStack.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/13/18.
//

#include "Selection_Stack.hpp"

#include <algorithm>

using namespace PolyVox;

template <class VoxelType>
void Selection_Stack<VoxelType>::add(Region region, VoxelType new_type, VoxelType old_type, SelectionType selection_type) {
    Selection<VoxelType> selection(region, new_type, old_type, selection_type);
    selections.push_back(selection);
}

template <class VoxelType>
void Selection_Stack<VoxelType>::updateEnclosingRegion(Region& region) {
    enclosing_region.setLowerX(max(enclosing_region.getLowerX(), region.getLowerX()));
    enclosing_region.setLowerY(max(enclosing_region.getLowerY(), region.getLowerY()));
    enclosing_region.setLowerZ(max(enclosing_region.getLowerZ(), region.getLowerZ()));
    
    enclosing_region.setUpperX(max(enclosing_region.getUpperX(), region.getUpperX()));
    enclosing_region.setUpperY(max(enclosing_region.getUpperY(), region.getUpperY()));
    enclosing_region.setUpperZ(max(enclosing_region.getUpperZ(), region.getUpperZ()));
}

template <class VoxelType>
static void setRegion(Region region, RawVolume<VoxelType> volume, VoxelType voxel) {
    for(int x = region.getLowerX(); x < region.getUpperX(); x++) {
        for(int y = region.getLowerY(); y < region.getUpperY(); y++) {
            for(int z = region.getUpperZ(); z < region.getUpperZ(); z++) {
                volume.setVoxel(x, y, z, voxel);
            }
        }
    }
}

template <class VoxelType>
static void replaceTypeInRegion(Region region, RawVolume<VoxelType> volume, VoxelType new_type, VoxelType old_type) {
    for(int x = region.getLowerX(); x < region.getUpperX(); x++) {
        for(int y = region.getLowerY(); y < region.getUpperY(); y++) {
            for(int z = region.getUpperZ(); z < region.getUpperZ(); z++) {
                if(volume.getVoxel(x, y, z) == old_type)  {
                    volume.setVoxel(x, y, z, new_type);
                }
            }
        }
    }
}

template <class VoxelType>
void Selection_Stack<VoxelType>::setVolumeFromSelection(Selection<VoxelType> &selection, RawVolume<VoxelType> volume) {
    switch(selection.selection_type) {
        case SELECTION_FILL:
            setRegion(selection.region, volume, selection.voxel_type);
            
            break;
        case SELECTION_REPLACE:
            replaceTypeInRegion(selection.region, volume, selection.voxel_type. selection.other_type);
            break;
    }
}



template <class VoxelType>
RawVolume<VoxelType> Selection_Stack<VoxelType>::flatten() {
    RawVolume<VoxelType> volume;
    
    for(Selection<VoxelType> selection : selections) {
        
    }
    
}


