#include "Application.hpp"

#include <iostream>

#include "WindowManager.h"

const int windowWidth = 800;
const int windowHeight = 420;

const float secondsPerOrb = 0.5f;

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
    windowManager->init(windowWidth, windowHeight);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;
    
    // This is the code that will likely change program to program as you
    // may need to initialize or set up different data and state
    
    application->init(resourceDir);
    
    double t = 0.0;
    double currentTime = glfwGetTime();
    double remainingTime = 20.0f;
    
    // Loop until the user closes the window.
    while (! glfwWindowShouldClose(windowManager->getHandle()))
    {
        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;
        
        //printf("Frame Rate: %f\tTime: %f\n", 1.0f / frameTime, frameTime);
        //printf("Time %f\n", t);
        
        //Simulate Scene
        application->simulate(frameTime);
        // Render scene.
        application->render();
        
        
        
        //Spawn orb every 'secondsPerOrb' seconds
        t += frameTime;
        remainingTime -= frameTime;
        if(t >= secondsPerOrb && application->gameOver == false) {
            t -= secondsPerOrb;
            printf("Framerate: %.01f\t", 1.0f / frameTime);
            printf("3D Objects: %d\n", application->score + application->freeOrbCount + 2);
            printf("Time Remaining: %f!\n", remainingTime);
            application->createOrb();
        }
        if(remainingTime <= 0.0f && application->gameOver == false) {
            printf("Game Over!\nScore: %d\n", application->score);
            application->gameOver = true;
            application->makeOrbsGreen();
        }
        
        
        
        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }
    
    // Quit program.
    windowManager->shutdown();
    return 0;
}

