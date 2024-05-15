//
// Created by Taylor Rodgers on 12/10/23.
//

#ifndef A5_WALLS_H
#define A5_WALLS_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

class Walls {
public:
    /// \desc creates a simple walls
    /// \param shaderProgramHandle shader program handle that the walls should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Walls(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    /// \desc draws the model walls for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to walls
    /// \param viewMtx camera view matrix to apply to walls
    /// \param projMtx camera projection matrix to apply to walls
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void drawWalls( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );
    [[nodiscard]] const glm::vec3 &getNorthWallPosition() const;
    [[nodiscard]] const glm::vec3 &getEastWallPosition() const;
    [[nodiscard]] const glm::vec3 &getSouthWallPosition() const;
    [[nodiscard]] const glm::vec3 &getWestWallPosition() const;

private:
    /// \desc handle of the shader program to use when drawing the walls
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

    glm::vec3 _northWallPosBig;
    glm::vec3 _eastWallPosBig;
    glm::vec3 _southWallPosBig;
    glm::vec3 _westWallPosBig;

    glm::vec3 _northWallPosSmall;
    glm::vec3 _eastWallPosSmall;
    glm::vec3 _southWallPosSmall;
    glm::vec3 _westWallPosSmall;

    glm::vec3 _scaleBigWallx;
    glm::vec3 _scaleBigWallz;
    glm::vec3 _scaleSmallWallx;
    glm::vec3 _scaleSmallWallz;

    glm::vec3 _colorWalls;

    std::vector<glm::vec3> _wallsLocation;

    void _drawSmallWall(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawBigWall(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};

#endif //A5_WALLS_H
