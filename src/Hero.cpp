#include "Hero.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

Hero::Hero(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _currPos = glm::vec3(-36, 2.2, -45);
    _falling = false;

    _yOffset = 0.1;
    _timeVariable = 0.0;
    _hoverAmount = 0.0;

    // Initializes all of our matrix calculations to draw our hero's body.
    _transWholeBody = glm::vec3( 0.0f, 2.2f, 0.0f);
    _scaleWholeBody = glm::vec3( 10.0f, 10.0f, 10.0f);
    _bodyAngle = 0.0f;
    _bodyAngleRotationFactor = _PI / 64;

    _colorHead = glm::vec3( 0.0f,0.5451f,0.5451f );
    _scaleHead = glm::vec3( 0.1f, 0.1f, 0.1f );
    _transHead = glm::vec3( 0.0f, 0.13f, 0.0f );

    _colorLeftEye = glm::vec3( 1.0f,1.0f,1.0f );
    _scaleLeftEye = glm::vec3( 0.1f, 0.1f, 0.1f );
    _transLeftEye = glm::vec3( 0.06f, 0.15f, 0.03f );

    _colorRightEye = glm::vec3( 1.0f,1.0f,1.0f );
    _scaleRightEye = glm::vec3( 0.1f, 0.1f, 0.1f );
    _transRightEye = glm::vec3( 0.06f, 0.15f, -0.03f );

    _colorBody = glm::vec3( 0.0f,0.5451f,0.5451f );
    _scaleBody = glm::vec3( 1.0f, 2.5f, 1.0f );
    _transBody = glm::vec3( 0.0f, -0.04f, 0.0f );

    _colorLegs = glm::vec3( 0.2f,0.2f,0.2f );
    _scaleLegs = glm::vec3( 1.1f, 2.0f, 1.1f );
    _transLegs = glm::vec3( 0.0f, -0.12f, 0.0f );

    _colorArm = glm::vec3( 0.8f, 0.8f, 0.8f );
    _scaleArm = glm::vec3(0.5f, 1.0f, 1.0f );
}

glm::vec3 Hero::getCurrPos() {
    return _currPos;
}

// Main function to put together the hero and draw it as a whole.
void Hero::drawHero(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    modelMtx = glm::translate(modelMtx, _currPos);
    modelMtx = glm::rotate( modelMtx, _bodyAngle, CSCI441::Y_AXIS );
    modelMtx = glm::scale( modelMtx, _scaleWholeBody );
    _drawHeroBody(modelMtx, viewMtx, projMtx);
    _drawHeroArm(modelMtx, viewMtx, projMtx);
    _drawHeroLegs(modelMtx, viewMtx, projMtx);
    _drawHeroHead(modelMtx, viewMtx, projMtx);
    _drawHeroLeftEye(modelMtx, viewMtx, projMtx);
    _drawHeroRightEye(modelMtx, viewMtx, projMtx);
}

bool Hero::getFalling() {
    return _falling;
}

void Hero::setFalling(bool falling) {
    _falling = falling;
}

// Implements our functions to turn our hero right and left.
void Hero::turnRight() {
    _bodyAngle -= _bodyAngleRotationFactor;
}

void Hero::turnLeft() {
    _bodyAngle += _bodyAngleRotationFactor;
}

void Hero::moveForward() {
    glm::vec3 nextPos = _currPos + glm::vec3(glm::cos(getBodyAngle()) / 10, 0.0, -glm::sin(getBodyAngle()) / 10);
    if (nextPos.x < 214 && nextPos.x > -214 && nextPos.z < 214 && nextPos.z > -214) {
        _currPos = nextPos;
    }
}

void Hero::moveBackward() {
    glm::vec3 nextPos = _currPos - glm::vec3(glm::cos(getBodyAngle()) / 10, 0.0, -glm::sin(getBodyAngle()) / 10);
    if (nextPos.x < 214 && nextPos.x > -214 && nextPos.z < 214 && nextPos.z > -214) {
        _currPos = nextPos;
    }
}

void Hero::idleMovement() {
    // Creates our idle movement of hovering up and down.
    _hoverAmount = _yOffset * std::sin(M_PI/180 * _timeVariable);
    _currPos.y += _hoverAmount * 0.1f;
    _timeVariable += 1;
    _currPos = glm::vec3(_currPos.x, _currPos.y, _currPos.z);
}

void Hero::setHeroPosition(glm::vec3 newPosition) {
    _currPos = newPosition;
}

void Hero::setHeroWinner() {
//    _colorHead = glm::vec3(1.0, 0.0, 0.0);
    _scaleWholeBody *= 1.01f;
}

void Hero::setHeroLoser() {
    _scaleWholeBody *= 0.99f;
}

const glm::vec3 &Hero::getBodySize() const {
    return _scaleWholeBody;
}

void Hero::setHeroSize() {
    _scaleWholeBody = glm::vec3( 15.0f, 15.0f, 15.0f);
    setHeroPosition(glm::vec3(_currPos.x, 3.3, _currPos.z));
}

void Hero::setHeroColor() {
    _colorBody = glm::vec3(1,0,0);
}

// Creates the function to correctly scale and draw our hero head using a sphere.
void Hero::_drawHeroHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transHead );
    modelMtx1 = glm::scale( modelMtx1, _scaleHead );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorHead[0]);

    CSCI441::drawSolidSphere( 0.8f, 10, 10);
}

// Creates the function to correctly scale and draw our hero eyes left and right using spheres.
void Hero::_drawHeroLeftEye(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transLeftEye );
    modelMtx1 = glm::scale( modelMtx1, _scaleLeftEye );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorLeftEye[0]);

    CSCI441::drawSolidSphere( 0.2f, 10, 10);
}

void Hero::_drawHeroRightEye(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transRightEye );
    modelMtx1 = glm::scale( modelMtx1, _scaleRightEye );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorRightEye[0]);

    CSCI441::drawSolidSphere( 0.2f, 10, 10);
}

// Creates the function to correctly scale and draw our hero's body using a cube.
void Hero::_drawHeroBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transBody );
    modelMtx1 = glm::scale( modelMtx1, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( 0.1f );
}

// Creates the function to correctly scale and draw our hero's legs using a cube.
void Hero::_drawHeroLegs(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transLegs );
    modelMtx1 = glm::scale( modelMtx1, _scaleLegs );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorLegs[0]);

    CSCI441::drawSolidCube( 0.1f );
}

// Creates the function to correctly scale and draw our hero's arms using a cube.
void Hero::_drawHeroArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale(modelMtx, _scaleArm );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorArm[0]);

    CSCI441::drawSolidCube( 0.17f );
}

void Hero::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
