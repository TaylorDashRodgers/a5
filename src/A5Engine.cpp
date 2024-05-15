#include "A5Engine.h"

#include <CSCI441/objects.hpp>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265f
#endif

//*************************************************************************************
//
// Public Interface

A5Engine::A5Engine()
         : CSCI441::OpenGLEngine(4, 1,
                                 1040, 880,
                                 "A3: The Cabin In The Woods") {

    for(auto& _key : _keys) _key = GL_FALSE;

    // Initialize some values for idle animation and enemy status.
    _currHeroHeight = 1.0f;
    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    countTiles = 0;
    enemy1Dead = false;
    enemy2Dead = false;
}

A5Engine::~A5Engine() {
    delete _pArcCam;
}

void A5Engine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS ) {
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;

            default: break; // suppress CLion warning
        }
    }
}

void A5Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void A5Engine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        // rotate the camera by the distance the mouse moved
        _pArcCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f, (_mousePosition.y - currMousePosition.y) * 0.005f );
    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

//*************************************************************************************
//
// Engine Setup

void A5Engine::mSetupGLFW() {
    CSCI441::OpenGLEngine::mSetupGLFW();

    // set our callbacks
    glfwSetKeyCallback(mpWindow, lab05_engine_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, lab05_engine_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, lab05_engine_cursor_callback);
}

void A5Engine::mSetupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                        // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	    // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	        // clear the frame buffer to black
}

void A5Engine::mSetupShaders() {
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/A3.v.glsl", "shaders/A3.f.glsl" );
    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.materialColor  = _lightingShaderProgram->getUniformLocation("materialColor");
    // TODO #3A: assign uniforms
    _lightingShaderUniformLocations.normalMatrix = _lightingShaderProgram->getUniformLocation("normalMatrix");
    _lightingShaderUniformLocations.lightDirection = _lightingShaderProgram->getUniformLocation("lightDirection");
    _lightingShaderUniformLocations.lightColor = _lightingShaderProgram->getUniformLocation("lightColor");

    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");
    // TODO #3B: assign attributes
    _lightingShaderAttributeLocations.vertexNormal = _lightingShaderProgram->getAttributeLocation("vertexNormal");

}

void A5Engine::mSetupBuffers() {
    // TODO #4: need to connect our 3D Object Library to our shader
    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vertexNormal );

    // TODO #5: give the hero the normal matrix location
    _pHero = new Hero(_lightingShaderProgram->getShaderProgramHandle(),
                      _lightingShaderUniformLocations.mvpMatrix,
                      _lightingShaderUniformLocations.normalMatrix,
                      _lightingShaderUniformLocations.materialColor);

    _pEnemy1 = new Enemy(_lightingShaderProgram->getShaderProgramHandle(),
                      _lightingShaderUniformLocations.mvpMatrix,
                      _lightingShaderUniformLocations.normalMatrix,
                      _lightingShaderUniformLocations.materialColor);

    _pEnemy2 = new Enemy(_lightingShaderProgram->getShaderProgramHandle(),
                         _lightingShaderUniformLocations.mvpMatrix,
                         _lightingShaderUniformLocations.normalMatrix,
                         _lightingShaderUniformLocations.materialColor);

    _pWalls = new Walls(_lightingShaderProgram->getShaderProgramHandle(),
                      _lightingShaderUniformLocations.mvpMatrix,
                      _lightingShaderUniformLocations.normalMatrix,
                      _lightingShaderUniformLocations.materialColor);

    // get hero position for cam look at.
    _currHeroPos = _pHero->getCurrPos();

    _createGroundBuffers();
    _generateEnvironment();
}

void A5Engine::_createGroundBuffers() {
    // TODO #8: expand our struct
    struct Vertex {
        GLfloat x, y, z;
        GLfloat nx, ny, nz;
    };

    // TODO #9: add normal data
    Vertex groundQuad[4] = {
            {-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
            { 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
            {-1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f},
            { 1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f}
    };

    GLushort indices[4] = {0,1,2,3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2];       // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);

    // TODO #10: hook up vertex normal attribute
    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vertexNormal);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void A5Engine::_generateEnvironment() {
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = 1.0f;
    const GLfloat GRID_LENGTH = 1.0f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    srand( time(0) );                                                   // seed our RNG

    // psych! everything's on a grid.
    for(int i = LEFT_END_POINT; i < RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        for(int j = BOTTOM_END_POINT; j < TOP_END_POINT; j += GRID_SPACING_LENGTH) {
            // don't just draw a tiles ANYWHERE.
            if( i % 2 && j % 2 ) {
                // translate to spot
                glm::mat4 transToSpotMtx = glm::translate( glm::mat4(1.0), glm::vec3(i, 0.0f, j) );

                // compute height
                GLdouble height = 0.3f;
                // scale to tile size
                glm::mat4 scaleToHeightMtx = glm::scale( glm::mat4(1.0), glm::vec3(9, height, 9) );

                // translate up to grid
                glm::mat4 transToHeight = glm::translate( glm::mat4(1.0), glm::vec3(0, height/2.0f, 0) );

                // compute full model matrix
                glm::mat4 modelMatrix = transToHeight * scaleToHeightMtx * transToSpotMtx;

                // compute color
                glm::vec3 color( 0.4f, 0.4f, 0.4f );
                // store tile properties
                TileData currentTile = {modelMatrix, color,glm::vec3(i*9, 0.0f, j*9)};
                _tiles.emplace_back(currentTile);
            }
        }
    }
}

void A5Engine::mSetupScene() {

    // Initialize Arcball Cam
    _pArcCam = new CSCI441::ArcballCam(5.0f, 15.0f);
    _pArcCam->setTheta(0.0f );
    _pArcCam->setPhi(1.9f );
    _pArcCam->setRadius( 10.0f );
    _pArcCam->setLookAtPoint(_currHeroPos + glm::vec3(0.0, _currHeroHeight, 0.0));
    _pArcCam->recomputeOrientation();

    // TODO #6: set lighting uniforms
    glm::vec3 lightDirection(1.0f, -1.0f, 1.0f);
    glm::vec3 lightColor(1.0f,1.0f,1.0f);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDirection, 1, &lightDirection[0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColor, 1, &lightColor[0]);

    _pEnemy1->setEnemyPosition(glm::vec3(45, 0, -45));
    _pEnemy2->setEnemyPosition(glm::vec3(-45, 0, 45));

    _pEnemy1->setEnemyHeading(_pEnemy1->getHeading() + _pEnemy1->getBodyAngleFactor() * 64);
    _pEnemy2->setEnemyHeading(_pEnemy2->getHeading() + _pEnemy2->getBodyAngleFactor() * 32);
}

//*************************************************************************************
//
// Engine Cleanup

void A5Engine::mCleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
}

void A5Engine::mCleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    delete _pHero;
    delete _pWalls;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void A5Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();

    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale( glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundColor(0.9f, 0.9f, 0.9f);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, groundColor);

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);
    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE TILES ////
    for( const TileData& currentTile : _tiles ) {
        _computeAndSendMatrixUniforms(currentTile.modelMatrix, viewMtx, projMtx);

        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, currentTile.color);

        CSCI441::drawSolidCube(1.0);
    }
    //// END DRAWING THE TILES ////

    //// BEGIN DRAWING THE HERO ////
    glm::mat4 modelMtx(1.0f);
    _pHero->drawHero(modelMtx, viewMtx, projMtx);
    if (_pHero->getFalling()) {
        _pHero->setHeroPosition(_pHero->getCurrPos() - glm::vec3(0, 0.3f, 0));
    }
    //// END DRAWING THE HERO ////

    //// BEGIN DRAWING THE ENEMIES ////
    if (!enemy1Dead) {
        _pEnemy1->drawEnemy(modelMtx, viewMtx, projMtx);
        if (_pEnemy1->getFalling()) {
            _pEnemy1->setEnemyPosition(_pEnemy1->getCurrPos() - glm::vec3(0, 0.3f, 0));
        }
    }

    if (!enemy2Dead) {
        _pEnemy2->drawEnemy(modelMtx, viewMtx, projMtx);
        if (_pEnemy2->getFalling()) {
            _pEnemy2->setEnemyPosition(_pEnemy2->getCurrPos() - glm::vec3(0, 0.3f, 0));
        }
    }
    //// END DRAWING THE ENEMIES ////

    //// BEGIN DRAWING THE WALLS ////
    _pWalls->drawWalls(modelMtx, viewMtx, projMtx);
    //// END DRAWING THE WALLS ////
}

void A5Engine::_updateScene() {
    // Handle the hero's forward movement and checks for environment boundaries.
    if(_keys[GLFW_KEY_W]) {
        _pHero->moveForward();
        isCollisionForward(_pHero->getCurrPos(), _pWalls->getNorthWallPosition(), _pWalls->getEastWallPosition(), _pWalls->getSouthWallPosition(), _pWalls->getWestWallPosition());
        if(_pHero->getCurrPos().x > 55.0f) {
            _pHero->setFalling(true);
        }
        if(_pHero->getCurrPos().x < -55.0f) {
            _pHero->setFalling(true);
        }
        if(_pHero->getCurrPos().z > 55.0f) {
            _pHero->setFalling(true);
        }
        if(_pHero->getCurrPos().z < -55.0f) {
            _pHero->setFalling(true);
        }
    }

    // Handle the hero's backward movement and checks for environment boundaries.
    if(_keys[GLFW_KEY_S]) {
        _pHero->moveBackward();
        isCollisionBackward(_pHero->getCurrPos(), _pWalls->getNorthWallPosition(), _pWalls->getEastWallPosition(), _pWalls->getSouthWallPosition(), _pWalls->getWestWallPosition());
        if(_pHero->getCurrPos().x > 55.0f) {
            _pHero->setFalling(true);
        }
        if(_pHero->getCurrPos().x < -55.0f) {
            _pHero->setFalling(true);
        }
        if(_pHero->getCurrPos().z > 55.0f) {
            _pHero->setFalling(true);
        }
        if(_pHero->getCurrPos().z < -55.0f) {
            _pHero->setFalling(true);
        }
    }

    // Rotates the hero's heading left or right.
    if(_keys[GLFW_KEY_D]) {
        _pHero->turnRight();
    }

    if(_keys[GLFW_KEY_A]) {
        _pHero->turnLeft();
    }

    // Zoom arcball cam in/out
    if ( _keys[GLFW_KEY_R]) {
        _pArcCam->moveForward(0.2f);
    }
    if ( _keys[GLFW_KEY_F]) {
        _pArcCam->moveBackward(0.2f);
    }

    // Create hero and enemy idle movements.
    _pHero->idleMovement();
    isOnTile(_pHero->getCurrPos());
    isWinner();

    _pEnemy1->idleMovement();
    _pEnemy2->idleMovement();

    // Check if the hero has fallen off the map a certain amount to close the game.
    if ( _pHero->getCurrPos().y < -50.0f ) {
        setWindowShouldClose();
    }

    // Creates the Enemy following the hero where ever he goes by checking the direction.
    enemy1DirectionToHero = _pHero->getCurrPos() - _pEnemy1->getCurrPos();
    enemy2DirectionToHero = _pHero->getCurrPos() - _pEnemy2->getCurrPos();

    enemy1DirectionToHero = normalize(enemy1DirectionToHero);
    enemy2DirectionToHero = normalize(enemy2DirectionToHero);

    _pEnemy1->setEnemyHeading(atan2(-enemy1DirectionToHero.z, enemy1DirectionToHero.x));
    _pEnemy1->moveForward();
    _pEnemy2->setEnemyHeading(atan2(-enemy2DirectionToHero.z, enemy2DirectionToHero.x));
    _pEnemy2->moveForward();

    // Checks for any collisions.
    isCollisionForwardEnemy1(_pEnemy1->getCurrPos(), _pWalls->getNorthWallPosition(), _pWalls->getEastWallPosition(), _pWalls->getSouthWallPosition(), _pWalls->getWestWallPosition());
    isCollisionForwardEnemy2(_pEnemy2->getCurrPos(), _pWalls->getNorthWallPosition(), _pWalls->getEastWallPosition(), _pWalls->getSouthWallPosition(), _pWalls->getWestWallPosition());
    isCollisionEnemies(_pEnemy1->getCurrPos(), _pEnemy2->getCurrPos());
    isCollisionEnemyHero(_pEnemy1->getCurrPos(), _pEnemy2->getCurrPos(), _pHero->getCurrPos());

    // Makes sure the enemy can fall off the map the same way the hero can.
    if(_pEnemy1->getCurrPos().x > 55.0f) {
        _pEnemy1->setFalling(true);
    }
    if(_pEnemy1->getCurrPos().x < -55.0f) {
        _pEnemy1->setFalling(true);
    }
    if(_pEnemy1->getCurrPos().z > 55.0f) {
        _pEnemy1->setFalling(true);
    }
    if(_pEnemy1->getCurrPos().z < -55.0f) {
        _pEnemy1->setFalling(true);
    }

    if(_pEnemy2->getCurrPos().x > 55.0f) {
        _pEnemy2->setFalling(true);
    }
    if(_pEnemy2->getCurrPos().x < -55.0f) {
        _pEnemy2->setFalling(true);
    }
    if(_pEnemy2->getCurrPos().z > 55.0f) {
        _pEnemy2->setFalling(true);
    }
    if(_pEnemy2->getCurrPos().z < -55.0f) {
        _pEnemy2->setFalling(true);
    }

    //// BEGIN UPDATING CAMERAS ////
    _updateCamPosition();
    //// END UPDATING CAMERAS ////
}

void A5Engine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(mpWindow) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( mpWindow, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // draw everything to the window
        _renderScene(_pArcCam->getViewMatrix(), _pArcCam->getProjectionMatrix());

        _updateScene();

        glfwSwapBuffers(mpWindow);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper FUnctions

void A5Engine::_updateCamPosition() {
    glm::vec3 lookAtPoint = _pHero->getCurrPos() + glm::vec3(0.0, 2.0, 0.0);

    _pArcCam->setLookAtPoint(lookAtPoint);
    _pArcCam->recomputeOrientation();
}

// Checks for tile collision to count up for the game and creates the goal of the game.
void A5Engine::isOnTile(glm::vec3 currPos) {
    for (TileData& currentTile : _tiles) {
        if (currPos.x > (currentTile._tileLocations.x - 4.5) && currPos.x < (currentTile._tileLocations.x + 4.5) && currPos.z > (currentTile._tileLocations.z - 4.5) && currPos.z < (currentTile._tileLocations.z + 4.5)) {
            currentTile.color = glm::vec3(0.0, 1.0, 0.0);
        }
    }
}

// Creates how the hero grows in size and kills the enemies if the hero has visited all tiles.
void A5Engine::isWinner() {
    for ( TileData& currentTile : _tiles ) {
        if ( currentTile.color.y == 1.0 ) {
            countTiles += 1;
        }
    }
    if ( countTiles == 36 ) {
        _pHero->setHeroWinner();
        enemy1Dead = true;
        enemy2Dead = true;
        if ( _pHero->getBodySize().x > 15.0f ) {
            _pHero->setHeroSize();
        }
    } else {
        countTiles = 0;
    }
}

// Creates the hero shrinking and closing the game if he gets touched by the enemy too much.
void A5Engine::isLoser() {
    _pHero->setHeroLoser();
    if ( _pHero->getBodySize().x < 1.0f ) {
        setWindowShouldClose();
    }
}

// Checks all the collisions for walls and hero and enemies.
void A5Engine::isCollisionForward(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall) {
    if (currPos.x > (northWall.x - 1.5) && currPos.x < (northWall.x + 1.5) && currPos.z > (northWall.z - 31.5) && currPos.z < (northWall.z + 31.5)) {
        _pHero->moveBackward();
    }

    if (currPos.x > (eastWall.x - 31.5) && currPos.x < (eastWall.x + 31.5) && currPos.z > (eastWall.z - 1.5) && currPos.z < (eastWall.z + 1.5)) {
        _pHero->moveBackward();
    }

    if (currPos.x > (southWall.x - 1.5) && currPos.x < (southWall.x + 1.5) && currPos.z > (southWall.z - 31.5) && currPos.z < (southWall.z + 31.5)) {
        _pHero->moveBackward();
    }

    if (currPos.x > (westWall.x - 31.5) && currPos.x < (westWall.x + 31.5) && currPos.z > (westWall.z - 1.5) && currPos.z < (westWall.z + 1.5)) {
        _pHero->moveBackward();
    }

    if (currPos.x > ((northWall.x / 2) - 1.5) && currPos.x < ((northWall.x / 2) + 1.5) && currPos.z > ((northWall.z / 2) - 13.5) && currPos.z < ((northWall.z / 2) + 13.5)) {
        _pHero->moveBackward();
    }

    if (currPos.x > ((eastWall.x / 2) - 13.5) && currPos.x < ((eastWall.x / 2) + 13.5) && currPos.z > ((eastWall.z / 2) - 1.5) && currPos.z < ((eastWall.z / 2) + 1.5)) {
        _pHero->moveBackward();
    }

    if (currPos.x > ((southWall.x / 2) - 1.5) && currPos.x < ((southWall.x / 2) + 1.5) && currPos.z > ((southWall.z / 2) - 13.5) && currPos.z < ((southWall.z / 2) + 13.5)) {
        _pHero->moveBackward();
    }

    if (currPos.x > ((westWall.x / 2) - 13.5) && currPos.x < ((westWall.x / 2) + 13.5) && currPos.z > ((westWall.z / 2) - 1.5) && currPos.z < ((westWall.z / 2) + 1.5)) {
        _pHero->moveBackward();
    }
}

void A5Engine::isCollisionForwardEnemy1(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall) {
    if (currPos.x > (northWall.x - 1.5) && currPos.x < (northWall.x + 1.5) && currPos.z > (northWall.z - 31.5) && currPos.z < (northWall.z + 31.5)) {
        _pEnemy1->moveBackward();
    }

    if (currPos.x > (eastWall.x - 31.5) && currPos.x < (eastWall.x + 31.5) && currPos.z > (eastWall.z - 1.5) && currPos.z < (eastWall.z + 1.5)) {
        _pEnemy1->moveBackward();
    }

    if (currPos.x > (southWall.x - 1.5) && currPos.x < (southWall.x + 1.5) && currPos.z > (southWall.z - 31.5) && currPos.z < (southWall.z + 31.5)) {
        _pEnemy1->moveBackward();
    }

    if (currPos.x > (westWall.x - 31.5) && currPos.x < (westWall.x + 31.5) && currPos.z > (westWall.z - 1.5) && currPos.z < (westWall.z + 1.5)) {
        _pEnemy1->moveBackward();
    }

    if (currPos.x > ((northWall.x / 2) - 1.5) && currPos.x < ((northWall.x / 2) + 1.5) && currPos.z > ((northWall.z / 2) - 13.5) && currPos.z < ((northWall.z / 2) + 13.5)) {
        _pEnemy1->moveBackward();
    }

    if (currPos.x > ((eastWall.x / 2) - 13.5) && currPos.x < ((eastWall.x / 2) + 13.5) && currPos.z > ((eastWall.z / 2) - 1.5) && currPos.z < ((eastWall.z / 2) + 1.5)) {
        _pEnemy1->moveBackward();
    }

    if (currPos.x > ((southWall.x / 2) - 1.5) && currPos.x < ((southWall.x / 2) + 1.5) && currPos.z > ((southWall.z / 2) - 13.5) && currPos.z < ((southWall.z / 2) + 13.5)) {
        _pEnemy1->moveBackward();
    }

    if (currPos.x > ((westWall.x / 2) - 13.5) && currPos.x < ((westWall.x / 2) + 13.5) && currPos.z > ((westWall.z / 2) - 1.5) && currPos.z < ((westWall.z / 2) + 1.5)) {
        _pEnemy1->moveBackward();
    }
}

void A5Engine::isCollisionForwardEnemy2(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall) {
    if (currPos.x > (northWall.x - 1.5) && currPos.x < (northWall.x + 1.5) && currPos.z > (northWall.z - 31.5) && currPos.z < (northWall.z + 31.5)) {
        _pEnemy2->moveBackward();
    }

    if (currPos.x > (eastWall.x - 31.5) && currPos.x < (eastWall.x + 31.5) && currPos.z > (eastWall.z - 1.5) && currPos.z < (eastWall.z + 1.5)) {
        _pEnemy2->moveBackward();
    }

    if (currPos.x > (southWall.x - 1.5) && currPos.x < (southWall.x + 1.5) && currPos.z > (southWall.z - 31.5) && currPos.z < (southWall.z + 31.5)) {
        _pEnemy2->moveBackward();
    }

    if (currPos.x > (westWall.x - 31.5) && currPos.x < (westWall.x + 31.5) && currPos.z > (westWall.z - 1.5) && currPos.z < (westWall.z + 1.5)) {
        _pEnemy2->moveBackward();
    }

    if (currPos.x > ((northWall.x / 2) - 1.5) && currPos.x < ((northWall.x / 2) + 1.5) && currPos.z > ((northWall.z / 2) - 13.5) && currPos.z < ((northWall.z / 2) + 13.5)) {
        _pEnemy2->moveBackward();
    }

    if (currPos.x > ((eastWall.x / 2) - 13.5) && currPos.x < ((eastWall.x / 2) + 13.5) && currPos.z > ((eastWall.z / 2) - 1.5) && currPos.z < ((eastWall.z / 2) + 1.5)) {
        _pEnemy2->moveBackward();
    }

    if (currPos.x > ((southWall.x / 2) - 1.5) && currPos.x < ((southWall.x / 2) + 1.5) && currPos.z > ((southWall.z / 2) - 13.5) && currPos.z < ((southWall.z / 2) + 13.5)) {
        _pEnemy2->moveBackward();
    }

    if (currPos.x > ((westWall.x / 2) - 13.5) && currPos.x < ((westWall.x / 2) + 13.5) && currPos.z > ((westWall.z / 2) - 1.5) && currPos.z < ((westWall.z / 2) + 1.5)) {
        _pEnemy2->moveBackward();
    }
}

void A5Engine::isCollisionBackward(glm::vec3 currPos, glm::vec3 northWall, glm::vec3 eastWall, glm::vec3 southWall, glm::vec3 westWall) {
    if (currPos.x > (northWall.x - 1.5) && currPos.x < (northWall.x + 1.5) && currPos.z > (northWall.z - 31.5) && currPos.z < (northWall.z + 31.5)) {
        _pHero->moveForward();
    }

    if (currPos.x > (eastWall.x - 31.5) && currPos.x < (eastWall.x + 31.5) && currPos.z > (eastWall.z - 1.5) && currPos.z < (eastWall.z + 1.5)) {
        _pHero->moveForward();
    }

    if (currPos.x > (southWall.x - 1.5) && currPos.x < (southWall.x + 1.5) && currPos.z > (southWall.z - 31.5) && currPos.z < (southWall.z + 31.5)) {
        _pHero->moveForward();
    }

    if (currPos.x > (westWall.x - 31.5) && currPos.x < (westWall.x + 31.5) && currPos.z > (westWall.z - 1.5) && currPos.z < (westWall.z + 1.5)) {
        _pHero->moveForward();
    }

    if (currPos.x > ((northWall.x / 2) - 1.5) && currPos.x < ((northWall.x / 2) + 1.5) && currPos.z > ((northWall.z / 2) - 13.5) && currPos.z < ((northWall.z / 2) + 13.5)) {
        _pHero->moveForward();
    }

    if (currPos.x > ((eastWall.x / 2) - 13.5) && currPos.x < ((eastWall.x / 2) + 13.5) && currPos.z > ((eastWall.z / 2) - 1.5) && currPos.z < ((eastWall.z / 2) + 1.5)) {
        _pHero->moveForward();
    }

    if (currPos.x > ((southWall.x / 2) - 1.5) && currPos.x < ((southWall.x / 2) + 1.5) && currPos.z > ((southWall.z / 2) - 13.5) && currPos.z < ((southWall.z / 2) + 13.5)) {
        _pHero->moveForward();
    }

    if (currPos.x > ((westWall.x / 2) - 13.5) && currPos.x < ((westWall.x / 2) + 13.5) && currPos.z > ((westWall.z / 2) - 1.5) && currPos.z < ((westWall.z / 2) + 1.5)) {
        _pHero->moveForward();
    }
}

void A5Engine::isCollisionEnemies(glm::vec3 currPosEnemy1, glm::vec3 currPosEnemy2) {
    if ((currPosEnemy1.x + 1 > currPosEnemy2.x - 1) && (currPosEnemy1.x - 1 < currPosEnemy2.x + 1) && (currPosEnemy1.z + 1 > currPosEnemy2.z - 1) && (currPosEnemy1.z - 1 < currPosEnemy2.z + 1)) {
        _pEnemy1->setEnemyColor(glm::vec3(0,0,1));
        enemy2Dead = true;
        _pEnemy1->setEnemySize();
//        _pEnemy2->setEnemyColor(glm::vec3(0,0,1));
    }
}

void A5Engine::isCollisionEnemyHero(glm::vec3 currPosEnemy1, glm::vec3 currPosEnemy2, glm::vec3 currPosHero) {
    if (((currPosEnemy1.x + 1 > currPosHero.x - 1) && (currPosEnemy1.x - 1 < currPosHero.x + 1) && (currPosEnemy1.z + 1 > currPosHero.z - 1) && (currPosEnemy1.z - 1 < currPosHero.z + 1)) || ((currPosEnemy2.x + 1 > currPosHero.x - 1) && (currPosEnemy2.x - 1 < currPosHero.x + 1) && (currPosEnemy2.z + 1 > currPosHero.z - 1) && (currPosEnemy2.z - 1 < currPosHero.z + 1))) {
        isLoser();
    }
}

void A5Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);

    // TODO #7: compute and send the normal matrix
    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normalMatrix, normalMtx);

}

//*************************************************************************************
//
// Callbacks

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (A5Engine*) glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (A5Engine*) glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (A5Engine*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}