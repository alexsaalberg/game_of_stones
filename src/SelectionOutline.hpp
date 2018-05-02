//
//  SelectionOutline.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/30/18.
//

#ifndef SelectionOutline_hpp
#define SelectionOutline_hpp

#include "PolyVox/Region.h"
#include "PolyVox/Vector.h"


class SelectionOutline {
public:
    SelectionOutline(int thickness, int resolution = 3);
    
    void setRegion(const PolyVox::Region& region);
    
    void setThickness(int numerator, int divisor);
private:
    PolyVox::Region region;
    int resolution;
};

#endif /* SelectionOutline_hpp */
