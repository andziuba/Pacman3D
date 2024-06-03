#include "game_logic.h"
#include "model.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <ctime> // For time()
#include <glm/glm.hpp> // For glm::vec3
#include <irrKlang.h>
using namespace irrklang;
#include <chrono>
#include <thread>


// IrrKlang engine
ISoundEngine* soundEngine;

glm::vec3 mazePosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 mazeFloorPosition = glm::vec3(0.0f, -0.5f, 0.0f);
glm::vec3 pacmanPosition = glm::vec3(0.0f, 0.0f, 2.5f);  // Initial position of Pacman
glm::vec3 ghostPositionPink = glm::vec3(0.0f, 0.0f, -0.2f);
glm::vec3 ghostPositionBlue = glm::vec3(-0.5f, 0.0f, -0.2f);
glm::vec3 ghostPositionRed = glm::vec3(0.0f, 0.0f, -1.1f);
glm::vec3 ghostPositionOrange = glm::vec3(0.5f, 0.0f, -0.2f);
glm::vec3 pointPosition = glm::vec3(0.5f, 0.0f, 2.3f);

const float pacmanSpeed = 2.0f;
const float ghostSpeed = 2.0f;
float pacmanSpeed_x = 0.0f;
float pacmanSpeed_y = 0.0f;
float detectionDistance = 0.18f; // Distance to check for collisions
float ghostDetectionDistance = 0.3f;

Direction lastPacmanDirection = RIGHT; // Initial direction
Direction desiredPacmanDirection = RIGHT; // Desired direction

Direction ghostDirections[4] = { UP, DOWN, LEFT, RIGHT };
enum GhostState { PREAPARING, NORMAL };
GhostState ghostStates[4] = { PREAPARING, PREAPARING, NORMAL, PREAPARING };

const glm::vec3 restrictedZoneMin(-1.0f, 0.0f, -1.0f); // Define the minimum corner of the restricted zone
const glm::vec3 restrictedZoneMax(1.0f, 0.0f, 1.0f); // Define the maximum corner of the restricted zone

bool isInRestrictedZone(const glm::vec3& position) {
    return (position.x > restrictedZoneMin.x && position.x < restrictedZoneMax.x &&
        position.z > restrictedZoneMin.z && position.z < restrictedZoneMax.z);
}


void stopPacman() {
    pacmanSpeed_x = 0.0f;
    pacmanSpeed_y = 0.0f;
}

void resetPacman(bool& gameStarted, bool& gameOver) {
    gameStarted = false;
    gameOver = true;

    soundEngine->stopAllSounds();
    soundEngine->play2D("resources/audio/pacman_death.wav", false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    soundEngine->stopAllSounds();
    soundEngine->play2D("resources/audio/pacman_beginning.wav", true);

    pacmanPosition = glm::vec3(0.0f, 0.0f, 2.3f);

    stopPacman();
    resetGhosts();

    lastPacmanDirection = RIGHT;
}

void resetGhosts() {
    ghostPositionPink = glm::vec3(0.0f, 0.0f, -0.2f);
    ghostPositionBlue = glm::vec3(-0.5f, 0.0f, -0.2f);
    ghostPositionRed = glm::vec3(0.0f, 0.0f, -1.1f);
    ghostPositionOrange = glm::vec3(0.5f, 0.0f, -0.2f);

    ghostStates[0] = PREAPARING;
    ghostStates[1] = PREAPARING;
    ghostStates[2] = NORMAL;
    ghostStates[3] = PREAPARING;
}

void handlePacmanControl(int key, int action) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W: // up
            desiredPacmanDirection = UP;
            break;
        case GLFW_KEY_S: // down
            desiredPacmanDirection = DOWN;
            break;
        case GLFW_KEY_A: // left
            desiredPacmanDirection = LEFT;
            break;
        case GLFW_KEY_D: // right
            desiredPacmanDirection = RIGHT;
            break;
        }
    }
}

bool isPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c, float& u, float& v) {
    glm::vec2 v0 = b - a;
    glm::vec2 v1 = c - a;
    glm::vec2 v2 = p - a;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    if (denom == 0.0f) {
        return false; // Degenerate triangle
    }

    u = (d11 * d20 - d01 * d21) / denom;
    v = (d00 * d21 - d01 * d20) / denom;
    float w = 1.0f - u - v;

    return (u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f);
}

bool checkCollision(const glm::vec3& position, const std::vector<float>& mazeVertices) {
    for (size_t i = 0; i < mazeVertices.size(); i += 9) {
        glm::vec3 v0(mazeVertices[i], mazeVertices[i + 1], mazeVertices[i + 2]);
        glm::vec3 v1(mazeVertices[i + 3], mazeVertices[i + 4], mazeVertices[i + 5]);
        glm::vec3 v2(mazeVertices[i + 6], mazeVertices[i + 7], mazeVertices[i + 8]);

        float u, v;
        if (isPointInTriangle(glm::vec2(position.x, position.z), glm::vec2(v0.x, v0.z), glm::vec2(v1.x, v1.z), glm::vec2(v2.x, v2.z), u, v)) {
            return true; // Collision detected
        }
    }
    return false; // No collision detected
}

void updatePacmanPosition(float deltaTime, const std::vector<float>& mazeVertices, bool& gameStarted, bool& gameOver) {
        if (pacmanPosition.x > 4.5f) {
            pacmanPosition.x = -4.3f;
        }
        else if (pacmanPosition.x < -4.5f) {
            pacmanPosition.x = 4.3f;
        }
        else {
            glm::vec3 potentialPosition = pacmanPosition;
            float newSpeed_x = 0.0f;
            float newSpeed_y = 0.0f;

            // Set potential new speed based on the desired direction
            switch (desiredPacmanDirection) {
            case UP:
                newSpeed_y = -pacmanSpeed;
                break;
            case DOWN:
                newSpeed_y = pacmanSpeed;
                break;
            case LEFT:
                newSpeed_x = -pacmanSpeed;
                break;
            case RIGHT:
                newSpeed_x = pacmanSpeed;
                break;
            }

            // Calculate potential new position
            potentialPosition += glm::vec3(newSpeed_x * deltaTime, 0.0f, newSpeed_y * deltaTime);

            // Check for collision in the desired direction using detectionDistance
            glm::vec3 detectionPosition = pacmanPosition + glm::vec3(newSpeed_x * detectionDistance, 0.0f, newSpeed_y * detectionDistance);
            if (!checkCollision(detectionPosition, mazeVertices)) {
                if (checkPacmanGhostCollision()) {
                    resetPacman(gameStarted, gameOver);
                }
                else {
                    // If no collision, update Pacman's position and speed
                    pacmanPosition = potentialPosition;
                    pacmanSpeed_x = newSpeed_x;
                    pacmanSpeed_y = newSpeed_y;
                    lastPacmanDirection = desiredPacmanDirection;
                }
            }
            else {
                // If there's a collision, try to continue in the last valid direction
                potentialPosition = pacmanPosition;
                newSpeed_x = 0.0f;
                newSpeed_y = 0.0f;

                switch (lastPacmanDirection) {
                case UP:
                    newSpeed_y = -pacmanSpeed;
                    break;
                case DOWN:
                    newSpeed_y = pacmanSpeed;
                    break;
                case LEFT:
                    newSpeed_x = -pacmanSpeed;
                    break;
                case RIGHT:
                    newSpeed_x = pacmanSpeed;
                    break;
                }

                // Check for collision in the last valid direction using detectionDistance
                detectionPosition = pacmanPosition + glm::vec3(newSpeed_x * detectionDistance, 0.0f, newSpeed_y * detectionDistance);
                potentialPosition = pacmanPosition + glm::vec3(newSpeed_x * deltaTime, 0.0f, newSpeed_y * deltaTime);
                if (!checkCollision(detectionPosition, mazeVertices)) {
                    // If no collision, update Pacman's position and continue in the last valid direction
                    if (checkPacmanGhostCollision()) {
                        resetPacman(gameStarted, gameOver);
                    }
                    else {
                        pacmanPosition = potentialPosition;
                        pacmanSpeed_x = newSpeed_x;
                        pacmanSpeed_y = newSpeed_y;
                    }
                }
                else {
                    // If there's a collision in the last valid direction as well, stop Pacman
                    stopPacman();
                }
            }
        }

}


void initRandom() {
    srand(static_cast<unsigned int>(time(nullptr)));
}

Direction getRandomDirection() {
    return ghostDirections[rand() % 4];
}

glm::vec3 getDirectionVector(Direction direction) {
    switch (direction) {
    case UP:
        return glm::vec3(0.0f, 0.0f, -1.0f);
    case DOWN:
        return glm::vec3(0.0f, 0.0f, 1.0f);
    case LEFT:
        return glm::vec3(-1.0f, 0.0f, 0.0f);
    case RIGHT:
        return glm::vec3(1.0f, 0.0f, 0.0f);
    default:
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

void updateGhostTransition(glm::vec3& ghostPosition, glm::vec3 targetPosition, float deltaTime) {
    glm::vec3 direction = glm::normalize(targetPosition - ghostPosition);
    glm::vec3 movement = direction * ghostSpeed * deltaTime;

    if (glm::length(targetPosition - ghostPosition) < glm::length(movement)) {
        ghostPosition = targetPosition;
    }
    else {
        ghostPosition += movement;
    }
}


void updateGhostPosition(glm::vec3& ghostPosition, Direction& currentDirection, const std::vector<float>& mazeVertices, float deltaTime) {
    glm::vec3 directionVector = getDirectionVector(currentDirection);
    glm::vec3 potentialPosition = ghostPosition + directionVector * ghostSpeed * deltaTime;

    // Check for collisions in the desired direction using detectionDistance
    glm::vec3 detectionPosition = ghostPosition + directionVector * ghostDetectionDistance;

    // Check if the potential position is in the restricted zone
    if (!checkCollision(detectionPosition, mazeVertices) && !isInRestrictedZone(potentialPosition)) {
        ghostPosition = potentialPosition;
    }
    else {
        // If a collision occurs or the ghost is trying to enter the restricted zone, choose a new random direction
        currentDirection = getRandomDirection();
    }

    // Wrap around the maze
    if (ghostPosition.x > 4.5f) {
        ghostPosition.x = -4.3f;
    }
    else if (ghostPosition.x < -4.5f) {
        ghostPosition.x = 4.3f;
    }
}


void updateGhostPositions(float deltaTime, const std::vector<float>& mazeVertices) {
    static Direction currentDirectionPink = getRandomDirection();
    static Direction currentDirectionBlue = getRandomDirection();
    static Direction currentDirectionRed = getRandomDirection();
    static Direction currentDirectionOrange = getRandomDirection();

    glm::vec3 redInitialPosition = glm::vec3(0.0f, 0.0f, -1.1f);

    // Handle Pink Ghost
    if (ghostStates[0] == PREAPARING) {
        updateGhostTransition(ghostPositionPink, redInitialPosition, deltaTime);
        if (ghostPositionPink == redInitialPosition) {
            ghostStates[0] = NORMAL;
        }
    }
    else {
        updateGhostPosition(ghostPositionPink, currentDirectionPink, mazeVertices, deltaTime);
    }

    // Handle Blue Ghost
    if (ghostStates[1] == PREAPARING) {
        updateGhostTransition(ghostPositionBlue, redInitialPosition, deltaTime);
        if (ghostPositionBlue == redInitialPosition) {
            ghostStates[1] = NORMAL;
        }
    }
    else {
        updateGhostPosition(ghostPositionBlue, currentDirectionBlue, mazeVertices, deltaTime);
    }

    // Handle Red Ghost
    if (ghostStates[2] == PREAPARING) {
        updateGhostTransition(ghostPositionRed, redInitialPosition, deltaTime);
        if (ghostPositionRed == redInitialPosition) {
            ghostStates[2] = NORMAL;
        }
    }
    else {
        updateGhostPosition(ghostPositionRed, currentDirectionRed, mazeVertices, deltaTime);
    }

    // Handle Orange Ghost
    if (ghostStates[3] == PREAPARING) {
        updateGhostTransition(ghostPositionOrange, redInitialPosition, deltaTime);
        if (ghostPositionOrange == redInitialPosition) {
            ghostStates[3] = NORMAL;
        }
    }
    else {
        updateGhostPosition(ghostPositionOrange, currentDirectionOrange, mazeVertices, deltaTime);
    }
}


bool detectCollision(glm::vec3 position1, glm::vec3 position2, float collisionDistance) {
    float distance = glm::distance(position1, position2);
    return distance < collisionDistance;
}

bool checkPacmanGhostCollision() {
    if (detectCollision(pacmanPosition, ghostPositionPink, detectionDistance)) {
        return true;
    }
    if (detectCollision(pacmanPosition, ghostPositionBlue, detectionDistance)) {
        return true;
    }
    if (detectCollision(pacmanPosition, ghostPositionRed, detectionDistance)) {
        return true;
    }
    if (detectCollision(pacmanPosition, ghostPositionOrange, detectionDistance)) {
        return true;
    }
    return false;
}
