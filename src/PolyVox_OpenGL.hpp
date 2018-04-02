//
//  PolyVox_OpenGL.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/26/18.
//

#ifndef PolyVox_OpenGL_hpp
#define PolyVox_OpenGL_hpp

#include <stdio.h>

#include <glad/glad.h>

#include "Program.h"

#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/Region.h"

#include "GLSL.h"


class PolyVox_OpenGL {
public:
    PolyVox::Mesh< PolyVox::CubicVertex<uint8_t> > mesh;
    GLuint indexBuffer;
    GLuint vertexBuffer;
    GLuint normalBuffer;
    GLuint textureBuffer;
    
    int beginIndex = 0;
    int endIndex = 0;
    
    void initCubicMesh(PolyVox::RawVolume<uint8_t>* volume);
    void initCubicMesh(std::shared_ptr<PolyVox::RawVolume<uint8_t> > volume);
    void initCubicMesh(std::shared_ptr<PolyVox::PagedVolume<uint8_t> > volume);
    void render(std::shared_ptr<Program> prog);
};

#endif /* PolyVox_OpenGL_hpp */
