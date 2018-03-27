//
//  Block.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/25/18.
//

#ifndef Block_hpp
#define Block_hpp

#include <stdio.h>

enum BlockType
{
    BlockType_Default = 0,
    
    BlockType_Grass,
    BlockType_Dirt,
    BlockType_Water,
    BlockType_Stone,
    BlockType_Wood,
    BlockType_Sand,
    
    BlockType_NumTypes,
};

class Block
{
public:
    Block();
    ~Block();
    
private:
    bool active;
    
    BlockType blockType;
};

#endif /* Block_hpp */
