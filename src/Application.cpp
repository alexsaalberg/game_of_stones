//
//  Application.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include "Application.hpp"
#include "stb_image.h"

using namespace std;
using namespace glm;
using namespace PolyVox;

void Application::init(const std::string& resourceDirectory) {
	currentState = make_shared<State>();
	previousState = make_shared<State>();
    entity_manager = make_shared<EntityManager>();
    
    input_system.entity_manager = entity_manager;
    
	initShaders(resourceDirectory+"/shaders");
    //initTextures(resourceDirectory+"/models");
    initGeom(resourceDirectory+"/models");
    initQuad();
    initCamera();
    initPlayer();
    
    /*
    vec3 base_pos = vec3(10.0f, 0.0f, 0.0f);
    vec3 pos = vec3(0.0f);
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            for(int k = 0; k < 10; k++) {
                pos = vec3(i, j, k);
                initHelicopter(base_pos + pos);
            }
        }
    }*/
    
    volData = make_shared<RawVolume<uint8_t>>(Region(0,0,0,63,63,63));
    createSphereInVolume(*volData.get(), 30.0f);
    voxel_rend.initCubicMesh(volData.get());
}

void Application::createSphereInVolume(RawVolume<uint8_t>& volData, float fRadius)
{
    //This vector hold the position of the center of the volume
    Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);
    
    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth(); z++)
    {
        for (int y = 0; y < volData.getHeight(); y++)
        {
            for (int x = 0; x < volData.getWidth(); x++)
            {
                //Store our current position as a vector...
                Vector3DFloat v3dCurrentPos(x,y,z);
                //And compute how far the current position is from the center of the volume
                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();
                
                uint8_t uVoxelValue = 0;
                
                
                //If the current voxel is less than 'radius' units from the center then we make it solid.
                if(fDistToCenter <= fRadius)
                {
                    //Our new voxel value
                    uVoxelValue = 255;
                }
                
                
                //Wrte the voxel value into the volume
                volData.setVoxel(x, y, z, uVoxelValue);
            }
        }
    }
}

void Application::initShaders(const std::string& resourceDirectory)
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    GLSL::checkVersion();
    glClearColor(.5f, .5f, .5f, 1.0f); // Set background color.
    glEnable(GL_DEPTH_TEST); // Enable z-buffer test.
    
    initMainProgram(resourceDirectory);
    initSimpleProgram(resourceDirectory);
}

void Application::initMainProgram(const std::string& resourceDirectory) {
    // Initialize the GLSL program.
    mainProgram = make_shared<Program>();
    mainProgram->setVerbose(true);
    mainProgram->setShaderNames(resourceDirectory + "/mainVert.glsl",
                                resourceDirectory + "/mainFrag.glsl");
    
    if (! mainProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    mainProgram->addUniform("P");
    mainProgram->addUniform("V");
    mainProgram->addUniform("M");
    mainProgram->addUniform("mAmbientCoefficient");
    mainProgram->addUniform("mDiffusionCoefficient");
    mainProgram->addUniform("mSpecularCoefficient");
    mainProgram->addUniform("mSpecularAlpha");
    mainProgram->addUniform("eyePosition");
    mainProgram->addUniform("directionTowardsLight");
    //mainProgram->addUniform("Texture0");
    mainProgram->addAttribute("vPosition");
    mainProgram->addAttribute("vNormal");
    mainProgram->addAttribute("vTextureCoordinates");
}


void Application::initSimpleProgram(const std::string& resourceDirectory) {
    // Initialize the GLSL program.
    simpleProgram = make_shared<Program>();
    simpleProgram->setVerbose(true);
    simpleProgram->setShaderNames(resourceDirectory + "/simple_vert.glsl",
                                resourceDirectory + "/simple_frag.glsl");
    
    if (! simpleProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    simpleProgram->addUniform("P");
    simpleProgram->addUniform("V");
}

void Application::initTextures(const std::string& resourceDirectory) {
    grassTexture = make_shared<Texture>();
    grassTexture->setFilename(resourceDirectory + "/water.jpg");
    grassTexture->init();
    grassTexture->setUnit(0);
    grassTexture->setWrapModes(GL_REPEAT, GL_REPEAT);
    //grassTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Application::initGeom(const std::string& resourceDirectory) {
    // this is the tiny obj shapes - not to be confused with our shapes
    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    
    string errStr;
    
    //load in the mesh and make the shapes
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               (resourceDirectory + "/sphere.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        sphereModel = make_shared<Model>();
        sphereModel->createModel(TOshapes, objMaterials);
    }
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                          (resourceDirectory + "/Helicopter.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        helicopterModel = make_shared<Model>();
        helicopterModel->createModel(TOshapes, objMaterials);
        helicopterModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        helicopterModel->scale *= 2.0f;
        printf("Helicopter Model!\n");
    }
}

void Application::initEntities() {
	initCamera();
}

void Application::initCamera() {
    //camera = make_shared<Camera>();
    camera_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(camera_id);
    Camera_Component* camera = entity_manager->add_component<Camera_Component>(camera_id);
    
    position->position = vec3(1.0f);
}

void Application::initPlayer() {
    player_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(player_id);
    Renderable_Component* renderable = entity_manager->add_component<Renderable_Component>(player_id);
    
    position->position = vec3(1.0f);
    renderable->model = helicopterModel;
}

void Application::initHelicopter(glm::vec3 position) {
    int id = entity_manager->create_entity();
    Position_Component* pos = entity_manager->add_component<Position_Component>(id);
    Renderable_Component* renderable = entity_manager->add_component<Renderable_Component>(id);
    
    pos->position = position;
    renderable->model = helicopterModel;
    //printf("HERE X%f\n", position.x);
}


/**** geometry set up for ground plane *****/
void Application::initQuad()
{
    GLuint VertexArrayID;
    //generate the VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    float unit = 1.0f;
    
    // A x-z plane at y = g_groundY of dim[-g_groundSize, g_groundSize]^2
    float CubeVertexPositions[] = {
        //top
        unit, unit, unit, //right, back   0
        -unit, unit, unit, //left, back   1
        -unit, unit, -unit, //left, front 2
        unit, unit, -unit, //right, front 3
        //bottom
        unit, -unit, unit, //right, back   4
        -unit, -unit, unit, //left, back   5
        -unit, -unit, -unit, //left, front 6
        unit, -unit, -unit, //right, front 7
    };
    
    unsigned short CubeIndices[] = {
        //top
        0,3,1,
        1,3,2,
        //bottom
        4,5,7,
        7,5,6,
        
        //front
        3, 7, 2,
        2, 7, 6,
        //back
        1, 5, 0,
        0, 5, 4,
        
        //0, 1, 4,
        //e4, 1, 5,
        
        //left
        2, 6, 1,
        1, 6, 5,
        
        //right
        0, 4, 3,
        3, 4, 7,
    };
   
    float CubeNormals[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0
    };
    
    /*
     float GroundTex[] = {
     0, 0, // back
     0, 1,
     1, 1,
     1, 0
     }; */
    
    
    /*float GroundTex[] = {
        0, 0, // back
        0, g_groundSize,
        g_groundSize, g_groundSize,
        g_groundSize, 0
    };*/
    
    gGiboLen = 6;
    
    glGenBuffers(1, &GroundBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, GroundBufferObject);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(GroundPos), GroundPos, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertexPositions), CubeVertexPositions, GL_STATIC_DRAW);
    
    //glGenBuffers(1, &GroundTextureBufferObject);
    //glBindBuffer(GL_ARRAY_BUFFER, GroundTextureBufferObject);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*10000*12, GroundTex, GL_STATIC_DRAW);
    
    //glGenBuffers(1, &GroundNormalBufferObject);
    //glBindBuffer(GL_ARRAY_BUFFER, GroundNormalBufferObject);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(GroundNorm), GroundNorm, GL_STATIC_DRAW);
    
    gGiboLen = 36;
    glGenBuffers(1, &GroundIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gGiboLen * sizeof(unsigned short), CubeIndices, GL_STATIC_DRAW);
}

void Application::renderGround(shared_ptr<Program> prog)
{
    int vertex_attribute = prog->getAttribute("vPosition");
    int normal_attribute = prog->getAttribute("vNormal");
    int texture_attribute = prog->getAttribute("vTextureCoordinates");
    
    CHECKED_GL_CALL( glEnableVertexAttribArray(vertex_attribute) );
    CHECKED_GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, GroundBufferObject) );
    CHECKED_GL_CALL( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0) );
    
    //glEnableVertexAttribArray(normal_attribute);
    //glBindBuffer(GL_ARRAY_BUFFER, GroundNormalBufferObject);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    //glEnableVertexAttribArray(texture_attribute);
    //glBindBuffer(GL_ARRAY_BUFFER, GroundTextureBufferObject);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // draw!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundIndexBufferObject);
    glDrawElements(GL_TRIANGLES, gGiboLen, GL_UNSIGNED_SHORT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, 10000*12);
    
    CHECKED_GL_CALL( glDisableVertexAttribArray(vertex_attribute) );
    CHECKED_GL_CALL( glDisableVertexAttribArray(normal_attribute) );
    //glDisableVertexAttribArray(texture_attribute);
}

void Application::integrate(float t, float dt) {
	//previousState = make_shared<State>( *currentState );
    
    //player->body->SetLinearVelocity(b2Vec2(15.0f, 0.0f));

    currentState->integrate(t, dt);

}

void Application::render(float t, float alpha) {
    //player = currentState->gameObjects.at(0);

	//camera->player = currentState->gameObjects.at(0);
    renderState(*currentState.get(), t);
}

void Application::renderGUI() {
    bool my_tool_active = true;
    // Create a window called "My First Tool", with a menu bar.
    ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
    
    static float my_color[4] = {0.0f};
    // Edit a color (stored as ~4 floats)
    ImGui::ColorEdit4("Color", my_color);
    
    // Plot some values
    const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
    ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));
    
    // Display contents in a scrolling region
    ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
    ImGui::BeginChild("Scrolling");
    for (int n = 0; n < 50; n++)
        ImGui::Text("%04d: Some text", n);
    ImGui::EndChild();
    ImGui::End();
}

void Application::renderState(State& state, float t) {
    render_system.draw(entity_manager, t, mainProgram);
}

void Application::initCubicMesh(shared_ptr<RawVolume<uint8_t> > volume) {
    Mesh< CubicVertex<uint8_t> > mesh;
    //extrac
    //DefaultIsQuadNeeded< uint8_t > quadNeeded;
    //mesh = extractCubicMesh(volume, ) 
    //CubicSurfaceExtractor< CubicVertex<uint8_t>, quadNeeded > extractor;
    mesh = PolyVox::extractCubicMesh(volume.get(), volume->getEnclosingRegion());//, quadNeeded, false );
    
    //CubicSurfaceExtractor< CubicVertex<uint8_t> > extractor();//(*volData.get(), volume->getEnclosingRegion(), &mesh);
}


// helper function to set materials for shading
void Application::SetMaterial(const std::shared_ptr<Program> prog, int i)
{
    CHECKED_GL_CALL( glUniform3f(prog->getUniform("mSpecularCoefficient"), 0.3f, 0.2f, 0.1f) );
    CHECKED_GL_CALL( glUniform1f(prog->getUniform("mSpecularAlpha"), 3.0f) );
    
    switch (i)
    {
        case 0: //shiny blue plastic
           glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.02f, 0.04f, 0.2f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.0f, 0.16f, 0.9f);;
            break;
        case 1: // flat grey
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.13f, 0.13f, 0.14f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.3f, 0.3f, 0.4f);
            break;
        case 2: //brass
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.3294f, 0.2235f, 0.02745f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7804f, 0.5686f, 0.11373f);
            break;
        case 3: //copper
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.1913f, 0.0735f, 0.0225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
            break;
        case 4: //green man
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.735f, 0.0225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.038f, 0.048f, 0.028f);
            break;
        case 5: //radiation
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.7, 0.7735f, 0.225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
            break;
        case 6: //stone
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.1735f, 0.1225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.438f, 0.4048f, 0.428f);
            break;
		case 7:
			glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.17f, 0.01f, 0.01f);
			glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.61f, 0.04f, 0.04f);
			break;
    }
}

//[0,1.0]
float Application::randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

//[-1.0, 1.0]
float Application::randomFloatNegativePossible() {
    return (randomFloat() * 2.0f) - 1.0f;
}
void Application::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
    
    float delta_angle = 5.0f;
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        camera->cameraPhi += delta_angle;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        camera->cameraPhi -= delta_angle;
    }
    
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        camera->cameraTheta += delta_angle;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        camera->cameraTheta -= delta_angle;
    }
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        camera->cameraDistance += delta_angle;
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        camera->cameraDistance -= delta_angle;
    }
}

//Todo: Remove these (Idk if they're being optimized out, but hopefully
//                    they're not being called every time the mouse moves)
void Application::scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
{}
void Application::mouseCallback(GLFWwindow *window, int button, int action, int mods)
{}
void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{}

void Application::resizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}
