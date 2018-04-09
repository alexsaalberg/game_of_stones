//
//  Camera.h
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#ifndef Camera_h
#define Camera_h

#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h" //CHECK_GL_CALL, among others

#include "MatrixStack.h"
#include "Component.hpp"

using namespace std;
using namespace glm;

class Camera {
public:
    static float aspect;
    
    static void setModelIdentityMatrix(shared_ptr<Program> program) {
        auto M = make_shared<MatrixStack>();
        M->pushMatrix();
        M->loadIdentity();
        CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())) );
        M->popMatrix();
    }

    static void setViewMatrix(Camera_Component* camera,  Position_Component* camera_position, std::shared_ptr<Program> program) {
        //std::shared_ptr<MatrixStack> V = Camera::getViewMatrix(camera, camera_position);
        glm::mat4 V = Camera::getViewMatrix(camera, camera_position);
        CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V) ) );
    }
    
    static void setProjectionMatrix(shared_ptr<Program> program) {
        glm::mat4 P = Camera::getProjectionMatrix();
        CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P)) );
    }
    
    static void setEyePosition(vec3 position, shared_ptr<Program> prog) {
        CHECKED_GL_CALL( glUniform3fv(prog->getUniform("eyePosition"), 1, value_ptr(position)) );
    }
    
    static void setLight(shared_ptr<Program> program) {
        vec3 directionFromLight = vec3(0.0f) - vec3(-5.0f, 50.0f, 10.0f); //from X to origin
        vec3 directionTowardsLight = -directionFromLight;
        CHECKED_GL_CALL( glUniform3f(program->getUniform("directionTowardsLight"), directionTowardsLight.x, directionTowardsLight.y, directionTowardsLight.z) );
    }
    
    static mat4 getViewMatrix(Camera_Component* camera, Position_Component* camera_position) {
        vec3 norot_identity = vec3(0.0f, 0.0f, 1.0f);
        
        vec3 position = camera_position->position;
        vec3 identity = camera_position->rotation * norot_identity;
        identity = position + identity;
        
        std::shared_ptr<MatrixStack> V = make_shared<MatrixStack>();
        V->pushMatrix();
        
        V->loadIdentity();
        V->lookAt(position, identity, vec3(0, 1, 0)); //Look at identity from position
        //V->translate(-1.0f * position); //Negative
        return V->topMatrix();
    }

    static mat4 getProjectionMatrix() {
        
        std::shared_ptr<MatrixStack> P = make_shared<MatrixStack>();
        P->pushMatrix();
        P->perspective(45.0f, aspect, 0.1f, 1000.0f);
        return P->topMatrix();
    }
    
};

#endif /* Camera_h */
