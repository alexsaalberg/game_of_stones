//
//  PolyVox_OpenGL.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/26/18.
//

#include "PolyVox_OpenGL.hpp"
#include "imgui_memory_editor.h"

using namespace PolyVox;
using namespace std;

void PolyVox_OpenGL::initCubicMesh(RawVolume<uint8_t>* volume) {
    mesh = PolyVox::extractCubicMesh(volume, volume->getEnclosingRegion());
    
    //Build an OpenGL index buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getNoOfIndices() * sizeof(unsigned int), mesh.getRawIndexData(), GL_STATIC_DRAW);
    
    //Build an OpenGL vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.getNoOfVertices() * sizeof(CubicVertex<uint8_t>), mesh.getRawVertexData(), GL_STATIC_DRAW);
    
    beginIndex = 0;
    endIndex = mesh.getNoOfIndices();
}

void PolyVox_OpenGL::initCubicMesh(shared_ptr<RawVolume<uint8_t> > volume) {
    mesh = PolyVox::extractCubicMesh(volume.get(), volume->getEnclosingRegion());
    
    //Build an OpenGL index buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getNoOfIndices() * sizeof(unsigned int), mesh.getRawIndexData(), GL_STATIC_DRAW);
    
    //Build an OpenGL vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.getNoOfVertices() * sizeof(CubicVertex<uint8_t>), mesh.getRawVertexData(), GL_STATIC_DRAW);
    
    beginIndex = 0;
    endIndex = mesh.getNoOfIndices();
    exit(-1);
}

void PolyVox_OpenGL::render(shared_ptr<Program> prog) {
    int vertex_attribute = prog->getAttribute("vPosition");
    
    CHECKED_GL_CALL(glEnableVertexAttribArray(vertex_attribute) );
    CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    CHECKED_GL_CALL(glVertexAttribPointer(vertex_attribute, 4, GL_BYTE, GL_FALSE, 0, (const void *)0));
    
    //Bind the vertex buffer
    CHECKED_GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer) );
    //glVertexPointer(3, GL_FLOAT, sizeof(CubicVertex<uint8_t>), 0);
    //glNormalPointer(GL_FLOAT, sizeof(CubicVertex<uint8_t>), (GLvoid*)12);
    
    //CHECKED_GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, GroundBufferObject) );
    //CHECKED_GL_CALL( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0) );
    
    //Bind the index buffer
    CHECKED_GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer) );
    //Draw with the index buffer
    CHECKED_GL_CALL( glDrawElements(GL_TRIANGLES, mesh.getNoOfIndices(), GL_UNSIGNED_INT, 0) );
    printf("VBuffer:%d\t", vertexBuffer);
    printf("VAttrib:%d\t", vertex_attribute);
    printf("IBuffer:%d\n", indexBuffer);
    printf("Just drew %d indices %d vertices\n", mesh.getNoOfIndices(), mesh.getNoOfVertices());
    
    static MemoryEditor mem_edit_1;                                            // store your state somewhere
    mem_edit_1.DrawWindow("Memory Editor", (uint8_t*)mesh.getRawIndexData(), mesh.getNoOfVertices(), 0x0000); // create a window and draw memory editor (if you already have a window, use DrawContents())
    
    CHECKED_GL_CALL( glDisableVertexAttribArray(vertex_attribute) );
    
    //CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    printf("\n");
}
