//
//  Camera.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/29/18.
//

#include "Camera.hpp"

using namespace std;
using namespace glm; //glm::radians() --> radians(); glm::vec3 --> vec3

void Camera::setModelIdentityMatrix(const std::shared_ptr<Program> prog) const {
    auto M = make_shared<MatrixStack>();
    M->pushMatrix();
        M->loadIdentity();
        CHECKED_GL_CALL( glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())) );
    M->popMatrix();
}


void Camera::setHelicopterViewMatrix(const std::shared_ptr<Program> prog) const {
    float x = cos(radians(cameraPhi))*cos(radians(cameraTheta));
    float y = sin(radians(cameraPhi));
    float z = cos(radians(cameraPhi))*sin(radians(cameraTheta));
    
    x = -cameraDistance;
    z = 0.0f;
    y = 0.0f;
    
    vec3 cameraIdentityVector = vec3(x, y, z);
    
    auto V = make_shared<MatrixStack>();
    V->pushMatrix();
    V->loadIdentity();
        V->lookAt(vec3(0, 0, 0), cameraIdentityVector, vec3(0, 1, 0));
        V->translate((-1.0f * player->position)); //Negative
        V->translate(vec3(-cameraDistance, 0.0f, 0.0f));
        CHECKED_GL_CALL( glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix())) );
    V->popMatrix();
}

void Camera::setViewMatrix(const std::shared_ptr<Program> prog) const {
    float x = cos(radians(cameraPhi))*cos(radians(cameraTheta));
    float y = sin(radians(cameraPhi));
    float z = cos(radians(cameraPhi))*sin(radians(cameraTheta));
    vec3 cameraIdentityVector = vec3(x, y, z);
    
    auto V = make_shared<MatrixStack>();
    V->pushMatrix();
    V->loadIdentity();
    V->lookAt(vec3(0.0f, 0.0f, 0.0f), cameraIdentityVector, vec3(0.0f, 1.0f, 0.0f));
    V->translate(-1.0f * player->position); //Negative
    CHECKED_GL_CALL( glUniformMatrix4fv(
                                        prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix())) );
    V->popMatrix();
}

void Camera::setProjectionMatrix(const std::shared_ptr<Program> prog, float aspect) const {
    auto P = make_shared<MatrixStack>();
    P->pushMatrix();
    P->perspective(45.0f, aspect, 0.01f, 100.0f);
    //P->perspective(45.0f, aspect, 0.05f, 100.0f);
    CHECKED_GL_CALL( glUniformMatrix4fv(
                                        prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())) );
    P->popMatrix();
}

void Camera::setEyePosition(const std::shared_ptr<Program> prog) const {
    CHECKED_GL_CALL( glUniform3f(prog->getUniform("eyePosition"), player->position.x + cameraDistance, player->position.y, player->position.z) );
}
