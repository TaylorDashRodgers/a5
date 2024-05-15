//
// Created by Taylor Rodgers on 12/11/23.
//

#ifndef A5_ENEMY_H
#define A5_ENEMY_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

class Enemy {
public:
    /// \desc creates a simple enemy
    /// \param shaderProgramHandle shader program handle that the enemy should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Enemy(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    /// \desc draws the model enemy for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to enemy
    /// \param viewMtx camera view matrix to apply to enemy
    /// \param projMtx camera projection matrix to apply to enemy
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void drawEnemy( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    glm::vec3 getCurrPos();
    GLfloat enemySpeed;
    GLfloat headingChangeRate;
    // Creates function to get our angle for use of moving forward and backward with heading.
    GLfloat getHeading() const { return _bodyAngle; }

    GLfloat getBodyAngleFactor() const { return _bodyAngleRotationFactor; }

    GLfloat getHeadingChange() const { return headingChangeRate; }

    bool getFalling();
    void setFalling(bool falling);

    // Initialize functions for turning right and left.
    void turnRight();
    void turnLeft();
    void moveForward();
    void moveBackward();
    void idleMovement();
    void setEnemyPosition(glm::vec3 newPosition);
    void setEnemyHeading(GLfloat newDirection);
    void setEnemyColor(glm::vec3 newColor);
//    [[nodiscard]] const glm::vec3 &getBodySize() const;
    void setEnemySize();

private:
    /// \desc handle of the shader program to use when drawing the enemy
    GLuint _shaderProgramHandle;
    /// \desc stores the uniform locations needed for the plan information
    struct ShaderProgramUniformLocations {
        /// \desc location of the precomputed ModelViewProjection matrix
        GLint mvpMtx;
        /// \desc location of the precomputed Normal matrix
        GLint normalMtx;
        /// \desc location of the material diffuse color
        GLint materialColor;
    } _shaderProgramUniformLocations;

    glm::vec3 _currPos;

    bool _falling;

    // Variables used to create idle motion of hovering.
    GLfloat _yOffset;
    GLfloat _timeVariable;
    GLfloat _hoverAmount;

    // Initialize variables for drawing the enemy.
    glm::vec3 _transWholeBody;
    glm::vec3 _scaleWholeBody;
    GLfloat _bodyAngle;
    GLfloat _bodyAngleRotationFactor;

    glm::vec3 _colorHead;
    glm::vec3 _scaleHead;
    glm::vec3 _transHead;

    glm::vec3 _colorLeftEye;
    glm::vec3 _scaleLeftEye;
    glm::vec3 _transLeftEye;

    glm::vec3 _colorRightEye;
    glm::vec3 _scaleRightEye;
    glm::vec3 _transRightEye;

    const GLfloat _PI = glm::pi<float>();

    // Initialize functions used to draw enemy parts.
    void _drawEnemyHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawEnemyLeftEye(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawEnemyRightEye(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};

#endif //A5_ENEMY_H
