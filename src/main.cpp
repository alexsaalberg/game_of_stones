#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

//QYBURN
#include "Actor.h"
#include "Player.h"
#include "Helper.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DDS_Loader.hpp"

#define PI 3.1415

using namespace std;
using namespace glm;

//QYBURN
const double pixelsToDegrees_X = 40;
const double pixelsToDegrees_Y = 40;

bool mouseDown = false;

class Application : public EventCallbacks
{
    
public:
    
    WindowManager * windowManager = nullptr;
    
    // Our shader program
    std::shared_ptr<Program> mainProgram;
    std::shared_ptr<Program> groundProgram;
    
    shared_ptr<Actor> temporaryActor;
    shared_ptr<Model> temporaryModel;
    
    vector<shared_ptr<Model>> models;
    vector<shared_ptr<Actor>> actors;
    
    shared_ptr<Player> player;
    
    shared_ptr<Texture> heightmapTexture;
    shared_ptr<Texture> grassTexture;
    
    float cHeight = 0.0f;
    
    double mouse_prevX;
    double mouse_prevY;
    
    //ground plane info
    GLuint GroundBufferObject, GroundNormalBufferObject, GroundTextureBufferObject, GroundIndexBufferObject;
    int gGiboLen;
    
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        else if (key == GLFW_KEY_A && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->moveLeft();
        }
        else if (key == GLFW_KEY_D && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->moveRight();
        }
        else if (key == GLFW_KEY_W && (action == GLFW_PRESS || GLFW_REPEAT ))
        {
            player->moveForward();
        }
        else if (key == GLFW_KEY_R && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->height += 0.1f;
        }
        else if (key == GLFW_KEY_F && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->height -= 0.1f;
        }
        else if (key == GLFW_KEY_S && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->moveBackward();
        }
        else if (key == GLFW_KEY_SPACE  && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->jump();
        }
        else if (key == GLFW_KEY_P  && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            printf("Position: %fx %fy %fz\n", player->position.x, player->position.y, player->position.z);
            printf("Angle: %ft %fp\n", player->cameraTheta, player->cameraPhi);
        }
    }
    
    void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
    {
        if(mouseDown) {
            player -> cameraTheta += (float) deltaX;
            player -> cameraPhi += (float) deltaY;
            
            player->restrictCamera();
        }
    }
    
    void mouseCallback(GLFWwindow *window, int button, int action, int mods)
    {
        double posX, posY;
        
        if (action == GLFW_PRESS)
        {
            mouseDown = !mouseDown; //Flip mouseDown;
            glfwGetCursorPos(window, &posX, &posY);
            
            if(mouseDown) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                mouse_prevX = posX;
                mouse_prevY = posY;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
    
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        double deltaX, deltaY;
        
        if(mouseDown) {
            //change in mouse pos since last
            deltaX = mouse_prevX - xpos;
            deltaY = mouse_prevY - ypos;
            mouse_prevX = xpos;
            mouse_prevY = ypos;
            
            //THIS LINE MAKES vertical moving mouse down MOVE camera up;
            deltaY *= -1;
            //THIS LINE MAKES horizontal mouse work as expected
            deltaX *= -1;
            player -> cameraTheta += (float) deltaX / pixelsToDegrees_X;
            player -> cameraPhi += (float) deltaY / pixelsToDegrees_Y;
        
            player->restrictCamera();
        }
    }
    
    void resizeCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
    
    void init(const std::string& resourceDirectory, const std::string& heightMapFileName) {
        initShaders(resourceDirectory+"/shaders");
        initTextures(resourceDirectory+"/models", heightMapFileName);
        initGeom(resourceDirectory+"/models");
        initQuad();
    }
    
    //code to set up the two shaders - a diffuse shader and texture mapping
    void initShaders(const std::string& resourceDirectory)
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        GLSL::checkVersion();
        // Set background color.
        glClearColor(.12f, .34f, .56f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        
        initMainProgram(resourceDirectory);
        initGroundProgram(resourceDirectory);
    }
    
    void initMainProgram(const std::string& resourceDirectory) {
        // Initialize the GLSL program.
        mainProgram = make_shared<Program>();
        mainProgram->setVerbose(true);
        mainProgram->setShaderNames(  resourceDirectory + "/heightmap_vert.glsl",
                                      resourceDirectory + "/heightmap_frag.glsl");
        mainProgram->setGeometryShaderName(resourceDirectory + "/heightmap_geom.glsl");
        
        if (! mainProgram->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        mainProgram->addUniform("P");
        mainProgram->addUniform("V");
        mainProgram->addUniform("M");
        mainProgram->addUniform("MaterialAmbientCoefficient");
        mainProgram->addUniform("MaterialDiffusionCoefficient");
        mainProgram->addUniform("MaterialSpecularCoefficient");
        mainProgram->addUniform("MaterialSpecularAlpha");
        mainProgram->addUniform("EyePosition");
        mainProgram->addUniform("Texture0");
        mainProgram->addAttribute("vertexPosition_modelSpace");
        mainProgram->addAttribute("vertexNormal");
        mainProgram->addAttribute("vertexTextureCoordinates");
    }
    
    void initGroundProgram(const std::string& resourceDirectory) {
        groundProgram = make_shared<Program>();
        groundProgram->setVerbose(true);
        groundProgram->setShaderNames(
                                resourceDirectory + "/tex_vert.glsl",
                                resourceDirectory + "/tex_frag0.glsl");
        if (! groundProgram->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        groundProgram->addUniform("P");
        groundProgram->addUniform("V");
        groundProgram->addUniform("M");
        groundProgram->addAttribute("vertPos");
        groundProgram->addAttribute("vertNor");
        groundProgram->addAttribute("vertTex");
        groundProgram->addUniform("Texture0");
    }
    
    void initTextures(const std::string& resourceDirectory, const std::string& heightMapFileName) {
        heightmapTexture = make_shared<Texture>();
        heightmapTexture->setFilename(resourceDirectory + heightMapFileName);
        heightmapTexture->init();
        heightmapTexture->setUnit(0);
        heightmapTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        
        grassTexture = make_shared<Texture>();
        grassTexture->setFilename(resourceDirectory + "/grass.jpg");
        grassTexture->init();
        grassTexture->setUnit(0);
        grassTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }
    
    void initGeom(const std::string& resourceDirectory) {
        // this is the tiny obj shapes - not to be confused with our shapes
        vector<tinyobj::shape_t> TOshapes;
        vector<tinyobj::material_t> objMaterials;
        
        string errStr;
        
        int texturePixelWidth = heightmapTexture->getWidth();
        int texturePixelHeight = heightmapTexture->getHeight();
        
        float diameter = (float) texturePixelWidth;
        float heightBetweenLayers = (1 / (float) texturePixelHeight);
        float scale = 2.0f;
        
        diameter *= scale;
        heightBetweenLayers *= scale;
        
        float radius = diameter / (2 * PI);
        
        printf("Diameter %f\t HeightBetween %f\n", diameter, heightBetweenLayers);
        printf("Width %d\t Height %d\n", texturePixelWidth, texturePixelHeight);
        
        shared_ptr<Shape> shape = make_shared<Shape>();
        shape->makeCylinder(texturePixelWidth, texturePixelHeight, radius, scale);
        temporaryModel = make_shared<Model>();
        temporaryModel->createModel(shape);
        
        temporaryActor = make_shared<Actor>();
        temporaryActor->createActor(temporaryModel);
        
        //temporaryActor->setPosition(vec3(0.0f, - 0.5f*scale, 0.0f));
                                    //+texturePixelHeight));
        temporaryActor->material = rand() % 6;
        //temporaryActor->addOffset(vec3(0, -2, 0));
        temporaryActor->scale(10.0f);
        
        actors.push_back(temporaryActor);
        
        player = make_shared<Player>();
        player->position.z += 4.0f;
        //player->height = texturePixelHeight/2
        player->cameraTheta = -90.0f;
        
    }
    
    /**** geometry set up for ground plane *****/
    void initQuad()
    {
        float g_groundSize = 20;
        float g_groundY = -1.5;
        
        // A x-z plane at y = g_groundY of dim[-g_groundSize, g_groundSize]^2
        float GroundPos[] = {
            -g_groundSize, g_groundY, -g_groundSize,
            -g_groundSize, g_groundY,  g_groundSize,
            g_groundSize, g_groundY,  g_groundSize,
            g_groundSize, g_groundY, -g_groundSize
        };
        
        float GroundNorm[] = {
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0
        };
        
        float GroundTex[] = {
            0, 0, // back
            0, 1,
            1, 1,
            1, 0
        };
        
        unsigned short idx[] = {0, 1, 2, 0, 2, 3};
        
        GLuint VertexArrayID;
        //generate the VAO
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        
        gGiboLen = 6;
        glGenBuffers(1, &GroundBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, GroundBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GroundPos), GroundPos, GL_STATIC_DRAW);
        
        glGenBuffers(1, &GroundNormalBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, GroundNormalBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GroundNorm), GroundNorm, GL_STATIC_DRAW);
        
        glGenBuffers(1, &GroundTextureBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, GroundTextureBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GroundTex), GroundTex, GL_STATIC_DRAW);
        
        glGenBuffers(1, &GroundIndexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundIndexBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    }
    
    void renderGround()
    {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, GroundBufferObject);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, GroundNormalBufferObject);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, GroundTextureBufferObject);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        
        // draw!
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundIndexBufferObject);
        glDrawElements(GL_TRIANGLES, gGiboLen, GL_UNSIGNED_SHORT, 0);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
    
    void render()
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(windowManager->getHandle(), &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
        float aspect = windowWidth/(float)windowHeight;
        
        CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
        CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
        CHECKED_GL_CALL( glDisable(GL_CULL_FACE) ) ; //default, two-sided rendering
        
        mainProgram->bind();
        // Apply perspective projection.
        
        player->step();
        player->setModelIdentityMatrix(mainProgram);
        player->setViewMatrix(mainProgram);
        player->setProjectionMatrix(mainProgram, aspect);
        
        player->setEyePosition(mainProgram);
        
        auto M = make_shared<MatrixStack>();
        M->pushMatrix();
            M->loadIdentity();
            M->scale(vec3(5.0f, 0.0f, 5.0f));
            CHECKED_GL_CALL( glUniformMatrix4fv(mainProgram->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())) );
        M->popMatrix();
        
        heightmapTexture->bind(mainProgram->getUniform("Texture0"));
        
        for(auto &actor : actors) {
            actor->step();
            SetMaterial(mainProgram, actor->material);
            actor->draw(mainProgram);
        }
        
        mainProgram->unbind();
        groundProgram->bind();
        
            player->setModelIdentityMatrix(groundProgram);
            player->setViewMatrix(groundProgram);
            player->setProjectionMatrix(groundProgram, aspect);
        
        
            /*draw the ground */
            grassTexture->bind(groundProgram->getUniform("Texture0"));
            renderGround();
        
        groundProgram->unbind();
        
    }
    
    // helper function to set materials for shading
    void SetMaterial(const std::shared_ptr<Program> prog, int i)
    {
        CHECKED_GL_CALL( glUniform3f(prog->getUniform("MaterialSpecularCoefficient"), 0.5f, 0.5f, 0.5f) );
        CHECKED_GL_CALL( glUniform1f(prog->getUniform("MaterialSpecularAlpha"), 8.0f) );
        
        switch (i)
        {
            case 0: //shiny blue plastic
                glUniform3f(prog->getUniform("MaterialAmbientCoefficient"), 0.02f, 0.04f, 0.2f);
                glUniform3f(prog->getUniform("MaterialDiffusionCoefficient"), 0.0f, 0.16f, 0.9f);;
                break;
            case 1: // flat grey
                glUniform3f(prog->getUniform("MaterialAmbientCoefficient"), 0.13f, 0.13f, 0.14f);
                glUniform3f(prog->getUniform("MaterialDiffusionCoefficient"), 0.3f, 0.3f, 0.4f);
                break;
            case 2: //brass
                glUniform3f(prog->getUniform("MaterialAmbientCoefficient"), 0.3294f, 0.2235f, 0.02745f);
                glUniform3f(prog->getUniform("MaterialDiffusionCoefficient"), 0.7804f, 0.5686f, 0.11373f);
                break;
            case 3: //copper
                glUniform3f(prog->getUniform("MaterialAmbientCoefficient"), 0.1913f, 0.0735f, 0.0225f);
                glUniform3f(prog->getUniform("MaterialDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
                break;
            case 4: //green man
                glUniform3f(prog->getUniform("MaterialAmbientCoefficient"), 0.0913f, 0.735f, 0.0225f);
                glUniform3f(prog->getUniform("MaterialDiffusionCoefficient"), 0.038f, 0.048f, 0.028f);
                break;
            case 5: //radiation
                glUniform3f(prog->getUniform("MaterialAmbientCoefficient"), 0.7, 0.7735f, 0.225f);
                glUniform3f(prog->getUniform("MaterialDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
                break;
            case 6: //stone
                glUniform3f(prog->getUniform("MaterialAmbientCoefficient"), 0.0913f, 0.1735f, 0.1225f);
                glUniform3f(prog->getUniform("MaterialDiffusionCoefficient"), 0.438f, 0.4048f, 0.428f);
                break;
        }
    }
    
};

int main(int argc, char **argv)
{
    // Where the resources are loaded from
    std::string resourceDir = "../resources";
    std::string heightMapFilename = "/mars_hm.jpg";
    
    if (argc >= 2)
    {
        resourceDir = argv[1];
    }
    
    if (argc >= 3)
    {
        heightMapFilename = argv[2];
    }
    
    
    Application *application = new Application();
    
    // Your main will always include a similar set up to establish your window
    // and GL context, etc.
    
    WindowManager *windowManager = new WindowManager();
    windowManager->init(512, 512);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;
    
    // This is the code that will likely change program to program as you
    // may need to initialize or set up different data and state
    
    application->init(resourceDir, heightMapFilename);
    
    // Loop until the user closes the window.
    while (! glfwWindowShouldClose(windowManager->getHandle()))
    {
        // Render scene.
        application->render();
        
        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }
    
    // Quit program.
    windowManager->shutdown();
    return 0;
}

