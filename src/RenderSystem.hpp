//
//  RenderSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//

#ifndef RenderSystem_h
#define RenderSystem_h

#include <memory> //shared_ptr

#include "System.hpp"

#include "EntityManager.hpp"
#include "WindowManager.h" //WindowManager
#include "Program.h"

struct VoxelMeshData
{
    GLuint noOfIndices;
    GLenum indexType;
    GLuint indexBuffer;
    GLuint vertexBuffer;
    GLuint vertexArrayObject;
    glm::vec3 translation;
    float scale;
    double dirty_time;
};

class RenderSystem : FrameSystem {
public:
    std::shared_ptr<EntityManager> entity_manager;
    WindowManager* window_manager;
    
    virtual void render(double t, std::shared_ptr<Program> program);
    //void draw(double t, std::shared_ptr<Program> program);
    void draw_entities(double t, std::shared_ptr<Program> program);
    
    //Camera Functions
    void setMVPE(double t, std::shared_ptr<Program> program);
    void renderGUI();
};

#endif /* RenderSystem_h */
