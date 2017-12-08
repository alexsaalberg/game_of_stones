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
    
    shared_ptr<Actor> temporaryActor;
    shared_ptr<Model> temporaryModel;
    
    vector<shared_ptr<Model>> models;
    vector<shared_ptr<Actor>> actors;
    
    shared_ptr<Player> player;
    
    
    float cHeight = 0.0f;
    
    double mouse_prevX;
    double mouse_prevY;
    
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
        else if (key == GLFW_KEY_S && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->moveBackward();
        }
        else if (key == GLFW_KEY_SPACE  && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->jump();
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
    
    //code to set up the two shaders - a diffuse shader and texture mapping
    void init(const std::string& resourceDirectory)
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        GLSL::checkVersion();
        // Set background color.
        glClearColor(.12f, .34f, .56f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        
        initMainProgram(resourceDirectory);
    }
    
    void initMainProgram(const std::string& resourceDirectory) {
        // Initialize the GLSL program.
        mainProgram = make_shared<Program>();
        mainProgram->setVerbose(true);
        mainProgram->setShaderNames(  resourceDirectory + "/directionalLight_vert.glsl",
                                      resourceDirectory + "/directionalLight_frag.glsl");
        
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
        mainProgram->addAttribute("vertexPosition_modelSpace");
        mainProgram->addAttribute("vertexNormal");
        mainProgram->addAttribute("vertexTextureCoordinates");
    }
    
    void initGeom(const std::string& resourceDirectory) {
        // this is the tiny obj shapes - not to be confused with our shapes
        vector<tinyobj::shape_t> TOshapes;
        vector<tinyobj::material_t> objMaterials;
        
        string errStr;
        
        //bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                                  // (resourceDirectory + "/dog.obj").c_str());
        bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                                   (resourceDirectory + "/pipeStraight.obj").c_str());
        
        if (!rc) {
            cerr << errStr << endl;
        } else {
            temporaryModel = make_shared<Model>();
            
            temporaryModel->createModel(TOshapes, objMaterials);
            //mTemp->rotate(vec3(-90, 0, 0));
        }
        
        //Initialize 100 dummies
        for(int i = 0; i < 100; i++) {
            temporaryActor = make_shared<Actor>();
            temporaryActor->createActor(temporaryModel);
            
            float rX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            temporaryActor->setPosition(vec3(rX*10.0f-5.0f, rY*10.0f, rZ*10.0f-5.0f));
            temporaryActor->material = rand() % 6;
            temporaryActor->addOffset(vec3(0, -2, 0));
            
            actors.push_back(temporaryActor);
        }
        
        player = make_shared<Player>();
    }
    
    void render()
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(windowManager->getHandle(), &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
        float aspect = windowWidth/(float)windowHeight;
        
        CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
        CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
        
        mainProgram->bind();
        // Apply perspective projection.
        
        player->step();
        player->setModelIdentityMatrix(mainProgram);
        player->setViewMatrix(mainProgram);
        player->setProjectionMatrix(mainProgram, aspect);
        
        player->setEyePosition(mainProgram);
        
        for(auto &actor : actors) {
            //actor->step();
            SetMaterial(mainProgram, actor->material);
            actor->draw(mainProgram);
        }
        
        mainProgram->unbind();
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
    
    if (argc >= 2)
    {
        resourceDir = argv[1];
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
    
    application->init(resourceDir+"/shaders");
    application->initGeom(resourceDir+"/models");
    
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

