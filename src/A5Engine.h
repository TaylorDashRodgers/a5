#ifndef LAB05_LAB05_ENGINE_H
#define LAB05_LAB05_ENGINE_H

#include <CSCI441/FreeCam.hpp>
#include <CSCI441/ArcballCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>

#include "Hero.h"
#include "Enemy.h"
#include "Walls.h"

#include <vector>

class A5Engine final : public CSCI441::OpenGLEngine {
public:
    A5Engine();
    ~A5Engine() final;

    void run() final;

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

    // Initialize variables for enemy tracking and dying.
    glm::vec3 enemy1DirectionToHero;
    glm::vec3 enemy2DirectionToHero;

    GLuint countTiles;
    bool enemy1Dead;
    bool enemy2Dead;

private:
    void mSetupGLFW() final;
    void mSetupOpenGL() final;
    void mSetupShaders() final;
    void mSetupBuffers() final;
    void mSetupScene() final;

    void mCleanupBuffers() final;
    void mCleanupShaders() final;

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const;
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();

    /// \desc tracks the number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with
    GLboolean _keys[NUM_KEYS];

    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;

//    /// \desc the static fixed camera in our world
//    CSCI441::ArcballCam* _pArcballCam;

    CSCI441::ArcballCam* _pArcCam;
    /// \desc pair of values to store the speed the camera can move/rotate.
    /// \brief x = forward/backward delta, y = rotational delta
    glm::vec2 _cameraSpeed;

    /// \desc our current hero's position
    glm::vec3 _currHeroPos;
    GLfloat _currHeroHeight;

    /// \desc our hero model
    Hero* _pHero;

    /// \desc our first enemy model
    Enemy* _pEnemy1;

    /// \desc our second enemy model
    Enemy* _pEnemy2;

    /// \desc our walls model
    Walls* _pWalls;

    /// \desc the size of the world (controls the ground size and locations of tiles)
    static constexpr GLfloat WORLD_SIZE = 55.0f;
    /// \desc VAO for our ground
    GLuint _groundVAO;
    /// \desc the number of points that make up our ground object
    GLsizei _numGroundPoints;

    /// \desc creates the ground VAO
    void _createGroundBuffers();

    /// \desc smart container to store information specific to each tile we wish to draw
    struct TileData {
        /// \desc transformations to position and size the tiles
        glm::mat4 modelMatrix;
        /// \desc color to draw the tiles
        glm::vec3 color;

        glm::vec3 _tileLocations;
    };
    /// \desc information list of all the tiles to draw
    std::vector<TileData> _tiles;

    /// \desc generates tiles information to make up our scene
    void _generateEnvironment();

    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram* _lightingShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct LightingShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc material diffuse color location
        GLint materialColor;
        // TODO #1: add new uniforms
        GLint normalMatrix;
        GLint lightDirection;
        GLint lightColor;


    } _lightingShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct LightingShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        // TODO #2: add new attributes
        GLint vertexNormal;

    } _lightingShaderAttributeLocations;

    void _updateCamPosition();

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    // Functions for how the game works and if you won or lost.
    void isOnTile(glm::vec3 currPos);
    void isWinner();
    void isLoser();

    // Functions for collision checking.
    void isCollisionForward(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall);
    void isCollisionForwardEnemy1(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall);
    void isCollisionForwardEnemy2(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall);
    void isCollisionBackward(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall);
    void isCollisionEnemies(glm::vec3 currPosEnemy1, glm::vec3 currPosEnemy2);
    void isCollisionEnemyHero(glm::vec3 currPosEnemy1, glm::vec3 currPosEnemy2, glm::vec3 currPosHero);
};

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y );
void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );

#endif// LAB05_LAB05_ENGINE_H
