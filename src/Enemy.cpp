//
// Created by Taylor Rodgers on 12/11/23.
//

#include "Enemy.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

Enemy::Enemy(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _currPos = glm::vec3(0, 0, 0);
    enemySpeed = 0.01f;
    headingChangeRate = 0.05f;
    _falling = false;

    _yOffset = 0.1;
    _timeVariable = 0.0;
    _hoverAmount = 0.0;

    // Initializes all of our matrix calculations to draw our enemy's body.
    _transWholeBody = glm::vec3( 0.0f, 2.2f, 0.0f);
    _scaleWholeBody = glm::vec3( 10.0f, 10.0f, 10.0f);
    _bodyAngle = 0.0f;
    _bodyAngleRotationFactor = _PI / 64;

    _colorHead = glm::vec3( 1.0f,0.0f,0.0f );
    _scaleHead = glm::vec3( 0.1f, 0.1f, 0.1f );
    _transHead = glm::vec3( 0.0f, 0.13f, 0.0f );

    _colorLeftEye = glm::vec3( 0.0f,0.0f,0.0f );
    _scaleLeftEye = glm::vec3( 0.1f, 0.1f, 0.1f );
    _transLeftEye = glm::vec3( 0.06f, 0.15f, 0.03f );

    _colorRightEye = glm::vec3( 0.0f,0.0f,0.0f );
    _scaleRightEye = glm::vec3( 0.1f, 0.1f, 0.1f );
    _transRightEye = glm::vec3( 0.06f, 0.15f, -0.03f );
}

glm::vec3 Enemy::getCurrPos() {
    return _currPos;
}

// Main function to put together the enemy and draw it as a whole.
void Enemy::drawEnemy(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    modelMtx = glm::translate(modelMtx, _currPos);
    modelMtx = glm::rotate( modelMtx, _bodyAngle, CSCI441::Y_AXIS );
    modelMtx = glm::scale( modelMtx, _scaleWholeBody );
    _drawEnemyHead(modelMtx, viewMtx, projMtx);
    _drawEnemyLeftEye(modelMtx, viewMtx, projMtx);
    _drawEnemyRightEye(modelMtx, viewMtx, projMtx);
}

bool Enemy::getFalling() {
    return _falling;
}

void Enemy::setFalling(bool falling) {
    _falling = falling;
}

// Implements our functions to turn our enemy right and left.
void Enemy::turnRight() {
    _bodyAngle -= _bodyAngleRotationFactor;
}

void Enemy::turnLeft() {
    _bodyAngle += _bodyAngleRotationFactor;
}

void Enemy::moveForward() {
    glm::vec3 nextPos = _currPos + glm::vec3(glm::cos(getHeading()) / 20, 0.0, -glm::sin(getHeading()) / 20);
    if (nextPos.x < 214 && nextPos.x > -214 && nextPos.z < 214 && nextPos.z > -214) {
        _currPos = nextPos;
    }
}

void Enemy::moveBackward() {
    glm::vec3 nextPos = _currPos - glm::vec3(glm::cos(getHeading()) / 20, 0.0, -glm::sin(getHeading()) / 20);
    if (nextPos.x < 214 && nextPos.x > -214 && nextPos.z < 214 && nextPos.z > -214) {
        _currPos = nextPos;
    }
}

void Enemy::idleMovement() {
    // Creates our idle movement of hovering up and down.
    _hoverAmount = _yOffset * std::sin(M_PI/180 * _timeVariable);
    _currPos.y += _hoverAmount * 0.1f;
    _timeVariable += 1;
    _currPos = glm::vec3(_currPos.x, _currPos.y, _currPos.z);
}

void Enemy::setEnemyPosition(glm::vec3 newPosition) {
    _currPos = newPosition;
}

void Enemy::setEnemyHeading(GLfloat newDirection) {
    _bodyAngle = newDirection;
}

void Enemy::setEnemyColor(glm::vec3 newColor) {
    _colorHead = newColor;
}

void Enemy::setEnemySize() {
    _scaleWholeBody = glm::vec3( 20.0f, 20.0f, 20.0f);
    setEnemyPosition(glm::vec3(_currPos.x, 1, _currPos.z));
}

// Creates the function to correctly scale and draw our enemy head using a sphere.
void Enemy::_drawEnemyHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transHead );
    modelMtx1 = glm::scale( modelMtx1, _scaleHead );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorHead[0]);

    CSCI441::drawSolidSphere( 0.8f, 10, 10);
}

// Creates the function to correctly scale and draw our enemy eyes left and right using spheres.
void Enemy::_drawEnemyLeftEye(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transLeftEye );
    modelMtx1 = glm::scale( modelMtx1, _scaleLeftEye );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorLeftEye[0]);

    CSCI441::drawSolidSphere( 0.2f, 10, 10);
}

void Enemy::_drawEnemyRightEye(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transRightEye );
    modelMtx1 = glm::scale( modelMtx1, _scaleRightEye );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorRightEye[0]);

    CSCI441::drawSolidSphere( 0.2f, 10, 10);
}

void Enemy::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}