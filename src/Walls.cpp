//
// Created by Taylor Rodgers on 12/10/23.
//

#include "Walls.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

Walls::Walls(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _northWallPosBig = glm::vec3(36,0,0);
    _eastWallPosBig = glm::vec3(0,0,36);
    _southWallPosBig = glm::vec3(-36,0,0);
    _westWallPosBig = glm::vec3(0,0,-36);

    _northWallPosSmall = glm::vec3(18,0,0);
    _eastWallPosSmall = glm::vec3(0,0,18);
    _southWallPosSmall = glm::vec3(-18,0,0);
    _westWallPosSmall = glm::vec3(0,0,-18);

    _scaleBigWallx = glm::vec3(63.0f, 5.0f, 3.0f);
    _scaleBigWallz = glm::vec3(3.0f, 5.0f, 63.0f);
    _scaleSmallWallx = glm::vec3(27.0f, 5.0f, 3.0f);
    _scaleSmallWallz = glm::vec3(3.0f, 5.0f, 27.0f);

    _colorWalls = glm::vec3(0.4f, 0.4f, 0.4f);
}

// Main function to put together the walls and draw it as a whole.
void Walls::drawWalls(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    _drawBigWall(modelMtx, viewMtx, projMtx);
    _drawSmallWall(modelMtx, viewMtx, projMtx);
}

const glm::vec3 &Walls::getNorthWallPosition() const {
    return _northWallPosBig;
}

const glm::vec3 &Walls::getEastWallPosition() const {
    return _eastWallPosBig;
}

const glm::vec3 &Walls::getSouthWallPosition() const {
    return _southWallPosBig;
}

const glm::vec3 &Walls::getWestWallPosition() const {
    return _westWallPosBig;
}

// Function to draw the big walls.
void Walls::_drawBigWall(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _northWallPosBig );
    modelMtx1 = glm::scale( modelMtx1, _scaleBigWallz );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );

    glm::mat4 modelMtx2 = glm::translate( modelMtx, _southWallPosBig );
    modelMtx2 = glm::scale( modelMtx2, _scaleBigWallz );

    _computeAndSendMatrixUniforms(modelMtx2, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );

    glm::mat4 modelMtx3 = glm::translate( modelMtx, _eastWallPosBig );
    modelMtx3 = glm::scale( modelMtx3, _scaleBigWallx );

    _computeAndSendMatrixUniforms(modelMtx3, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );

    glm::mat4 modelMtx4 = glm::translate( modelMtx, _westWallPosBig );
    modelMtx4 = glm::scale( modelMtx4, _scaleBigWallx );

    _computeAndSendMatrixUniforms(modelMtx4, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );
}

// Function to draw the small walls.
void Walls::_drawSmallWall(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _northWallPosSmall );
    modelMtx1 = glm::scale( modelMtx1, _scaleSmallWallz );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );

    glm::mat4 modelMtx2 = glm::translate( modelMtx, _southWallPosSmall );
    modelMtx2 = glm::scale( modelMtx2, _scaleSmallWallz );

    _computeAndSendMatrixUniforms(modelMtx2, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );

    glm::mat4 modelMtx3 = glm::translate( modelMtx, _eastWallPosSmall );
    modelMtx3 = glm::scale( modelMtx3, _scaleSmallWallx );

    _computeAndSendMatrixUniforms(modelMtx3, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );

    glm::mat4 modelMtx4 = glm::translate( modelMtx, _westWallPosSmall );
    modelMtx4 = glm::scale( modelMtx4, _scaleSmallWallx );

    _computeAndSendMatrixUniforms(modelMtx4, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWalls[0]);

    CSCI441::drawSolidCube( 1.0 );
}

void Walls::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}