#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <iostream>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <irrKlang.h>
using namespace irrklang;

#include "game_logic.h"
#include "model.h"
#include "positions.h"

extern ISoundEngine* soundEngine;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum GhostState { PREAPARING, NORMAL };

// Pacman
extern glm::vec3 pacmanPosition;
extern const float pacmanSpeed;
extern float pacmanSpeed_x;
extern float pacmanSpeed_y;
extern float detectionDistance;
extern float pointDetectionDistance;
extern Direction lastPacmanDirection;
extern Direction desiredPacmanDirection;

// Duszki
extern glm::vec3 ghostPositionRed;
extern glm::vec3 ghostPositionBlue;
extern glm::vec3 ghostPositionPink;
extern glm::vec3 ghostPositionOrange;
extern const float ghostSpeed;
extern float ghostDetectionDistance;
extern GhostState ghostStates[4];

void stopPacman();
void resetGhosts();
void resetGame(bool& gameStarted, bool& gameOver);
void handlePacmanControl(int key, int action);
bool isPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c, float& u, float& v);
bool checkWallCollision(const glm::vec3& position, const std::vector<float>& mazeVertices);
bool detectCollision(glm::vec3 position1, glm::vec3 position2, float collisionDistance);
bool checkPacmanGhostCollision();
bool checkPacmanPointCollision();
void updatePacmanPosition(float deltaTime, const std::vector<float>& mazeVertices, bool& gameStarted, bool& gameOver);
Direction getRandomDirection();
glm::vec3 getDirectionVector(Direction direction);
void updateGhostTransition(glm::vec3& ghostPosition, glm::vec3 targetPosition, float deltaTime);
bool isInRestrictedZone(const glm::vec3& position);
void updateGhostPosition(glm::vec3& ghostPosition, Direction& currentDirection, const std::vector<float>& mazeVertices, float deltaTime);
void updateGhostPositions(float deltaTime, const std::vector<float>& mazeVertices, bool& gameStarted, bool& gameOver);

#endif
