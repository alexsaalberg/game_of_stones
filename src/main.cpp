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
    double accumulator = 0.0f;
    
    float simulationsPerSecond = 60.0f;
    float dt = 1.0f / simulationsPerSecond;
    
    //double remainingTime = 20.0f;
    int numSimulationsThisFrame = 0;
    
    // Loop until the user closes the window.
    while (! glfwWindowShouldClose(windowManager->getHandle()))
    {
        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;
        
        printf("Frame Rate: %f\tTime: %f\n", 1.0f / frameTime, frameTime);
        
        /*
        if (frameTime>0.25f)
            frameTime = 0.25f;*/
        
        accumulator += frameTime;
        
        numSimulationsThisFrame = 0;
        while (accumulator>=dt)
        {
            accumulator -= dt;
            
            application->simulate(dt);
            //integrate(current, t, dt);
            t += dt;
            numSimulationsThisFrame++;
        }
        printf("Num Simulations: %d\n", numSimulationsThisFrame);
        
        //State state = interpolate(previous, current, accumulator/dt);
        
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

