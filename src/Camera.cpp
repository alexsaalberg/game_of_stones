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

std::shared_ptr<MatrixStack> Camera::getViewMatrix() {
    float x = cos(radians(cameraPhi))*cos(radians(cameraTheta));
    float y = sin(radians(cameraPhi));
    float z = cos(radians(cameraPhi))*sin(radians(cameraTheta));
    
    //x = 1.0f;
    //z = cameraDistance;
    //y = 5.0f;
    vec3 identityVector = vec3(0.0f) - vec3(x, y, z); //from origin to xyz
    identityVector *= cameraDistance;
    
    vec3 offsetVector = vec3(10.0f, 0.0f, 0.0f);
    
    std::shared_ptr<MatrixStack> V = make_shared<MatrixStack>();
    V->pushMatrix();
        V->loadIdentity();
        V->lookAt(vec3(0, 0, 0), identityVector, vec3(0, 1, 0));
        V->translate((-1.0f * vec3(player->position.x, 0.0f, 0.0f))); //Negative
        V->translate(identityVector);
        return V;
}

void Camera::setViewMatrix(const std::shared_ptr<Program> prog) {
    std::shared_ptr<MatrixStack> V = getViewMatrix();
    
    CHECKED_GL_CALL( glUniformMatrix4fv(
                prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()) ) );
}

std::shared_ptr<MatrixStack> Camera::getProjectionMatrix(float aspect) {
    std::shared_ptr<MatrixStack> P = make_shared<MatrixStack>();
    P->pushMatrix();
        P->perspective(45.0f, aspect, 0.1f, 1000.0f);
        return P;
}

void Camera::setProjectionMatrix(const std::shared_ptr<Program> prog, float aspect) const {
    auto P = make_shared<MatrixStack>();
    P->pushMatrix();
    P->perspective(45.0f, aspect, 0.1f, 1000.0f);
    //P->perspective(45.0f, aspect, 0.05f, 100.0f);
    CHECKED_GL_CALL( glUniformMatrix4fv(
                                        prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())) );
    P->popMatrix();
}

void Camera::setOrthogonalMatrix(const std::shared_ptr<Program> prog, float aspect) const {
    auto P = make_shared<MatrixStack>();
    P->pushMatrix();
    
    float horz = 100.0f;
    float vert = 100.0f;
    float near = 0.1f;
    float far = 100.0f;
    
    P->ortho(-horz, horz, -vert, vert, near, far);
    CHECKED_GL_CALL( glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())) );
    P->popMatrix();
}

void Camera::setEyePosition(const std::shared_ptr<Program> prog) const {
    CHECKED_GL_CALL( glUniform3f(prog->getUniform("eyePosition"), player->position.x + cameraDistance, player->position.y, player->position.z) );
}
