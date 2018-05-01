//
//  SelectionOutline.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/30/18.
//

#include "SelectionOutline.hpp"

#include <algorithm>    // std::min

#include "CastleDef.h"
#include "PolyVox/RawVolume.h"

using namespace PolyVox;

SelectionOutline::SelectionOutline(int thickness, int resolution) {
    PolyVox::Region model_region(0, 0, 0, resolution, resolution, resolution);
    
    PolyVox::RawVolume<CASTLE_VOXELTYPE> model_volume(model_region);
    
    for(int x = 0; x < resolution; x++) {
        for(int y = 0; y < resolution; y++) {
            for(int z = 0; z < resolution; z++) {
                int x_distance = std::min(x, resolution - x);
                int y_distance = std::min(y, resolution - y);
                int z_distance = std::min(z, resolution - z);
                
                int distance_to_edge = std::max(x_distance, y_distance);
                distance_to_edge = std::max(distance_to_edge, z_distance);
                
                if(distance_to_edge <= thickness) {
                    model_volume.setVoxel(x, y, z, 1);
                }
            }
        }
    }
    
}

void SelectionOutline::setRegion(const Region& region) {
    this->region = region;
}
