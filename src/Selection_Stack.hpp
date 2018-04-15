//
//  SelectionStack.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/13/18.
//

#ifndef SelectionStack_hpp
#define SelectionStack_hpp

#include <vector>

#include "PolyVox/Region.h"
#include "PolyVox/RawVolume.h"

enum SelectionType{SELECTION_FILL, SELECTION_REPLACE};

template <class VoxelType>
struct Selection {
    PolyVox::Region region;
    VoxelType new_voxel;
    VoxelType old_voxel;
    SelectionType selection_type;
};

template <class VoxelType>
class Selection_Stack {
public:
    void add(PolyVox::Region, VoxelType, VoxelType, SelectionType);
    void updateEnclosingRegion(PolyVox::Region& region);
    
    static void setVolumeFromSelection(Selection<VoxelType> &selection, PolyVox::RawVolume<VoxelType> volume);
    
    PolyVox::RawVolume<VoxelType> flatten();
private:
    std::vector<Selection<VoxelType> > selections;
    PolyVox::Region enclosing_region;
};

#endif /* SelectionStack_hpp */
