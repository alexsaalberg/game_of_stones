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

    
}

void SelectionOutline::setRegion(const Region& region) {
    this->region = region;
}
