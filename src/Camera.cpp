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

void Camera::setHelicopterSkyViewMatrix(const std::shared_ptr<Program> prog) {
    float x = 0.0f;
    float z = cameraDistance;
    float y = 0.0f;
    vec3 identityVector = vec3(0.0f) - vec3(x, y, z); //from origin to xyz
    
    vec3 offsetVector = vec3(10.0f, 0.0f, 0.0f);
    
    auto V = make_shared<MatrixStack>();
    V->pushMatrix();
    V->loadIdentity();
    V->lookAt(vec3(0, 0, 0), identityVector, vec3(0, 1, 0));
    V->rotate(cameraRot, vec3(1, 0, 0));
    V->translate((-1.0f * vec3(player->position.x, 0.0f, 0.0f) )); //Negative
    V->translate(identityVector - offsetVector);
    //V->translate(vec3(0.0f, 0.0f, -cameraDistance));
    CHECKED_GL_CALL( glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix())) );
    V->popMatrix();
    
    changeRot();
}

std::shared_ptr<MatrixStack> Camera::getHelicopterViewMatrix() {
    float x = cos(radians(cameraPhi))*cos(radians(cameraTheta));
    float y = sin(radians(cameraPhi));
    float z = cos(radians(cameraPhi))*sin(radians(cameraTheta));
    
    x = 1.0f;
    z = cameraDistance;
    y = 5.0f;
    vec3 identityVector = vec3(0.0f) - vec3(x, y, z); //from origin to xyz
    
    vec3 offsetVector = vec3(10.0f, 0.0f, 0.0f);
    
    std::shared_ptr<MatrixStack> V = make_shared<MatrixStack>();
    V->pushMatrix();
        V->loadIdentity();
        V->lookAt(vec3(0, 0, 0), identityVector, vec3(0, 1, 0));
        V->translate((-1.0f * vec3(player->position.x, 0.0f, 0.0f) )); //Negative
        V->translate(identityVector - offsetVector);
        V->rotate(cameraRot, vec3(1, 0, 0));
        return V;
}

void Camera::setHelicopterViewMatrix(const std::shared_ptr<Program> prog) {
    std::shared_ptr<MatrixStack> V = getHelicopterViewMatrix();
    
    CHECKED_GL_CALL( glUniformMatrix4fv(
                prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()) ) );

    changeRot();
}

void Camera::changeRot() {
    if(gameStarted) {
        if(cameraRot > 0) {
            cameraRot -= 0.0015f;
        }
        if(cameraDistance < 25.0f) {
            cameraDistance += 0.03f;
        }
    }
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

std::shared_ptr<MatrixStack> Camera::getProjectionMatrix(float aspect) {
    std::shared_ptr<MatrixStack> P = make_shared<MatrixStack>();
    P->pushMatrix();
        P->perspective(45.0f, aspect, 0.01f, 100.0f);
        return P;
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

vec2 Camera::getXBounds(float aspect) {
    /*
    std::shared_ptr<MatrixStack> V = getHelicopterViewMatrix();
    std::shared_ptr<MatrixStack> P = getProjectionMatrix(aspect);
    
    mat4 invV = inverse(V->topMatrix());
    mat4 invP = inverse(P->topMatrix());
    
    vec4 leftX = vec4(0.0f, 0.5f, 0.5f, 1.0f);
    vec4 rightX = vec4(1.0f, 0.5f, 0.5f, 1.0f);
    
    leftX = invV * invP * leftX;
    rightX = invV * invP * rightX;
    
    leftX /= leftX.w;
    rightX /= rightX.w;
     */
    
    float leftXBound = player->position.x - 20.0f;
    float rightXBound = player->position.x + 50.0f;
    
    
    return vec2(leftXBound, rightXBound);
}
