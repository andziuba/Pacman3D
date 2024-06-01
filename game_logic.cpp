#include "game_logic.h"
#include "model.h"
#include <GLFW/glfw3.h>
#include <iostream>

glm::vec3 mazePosition = glm::vec3(0.0f, -0.3f, 0.0f);
glm::vec3 pacmanPosition = glm::vec3(0.0f, 0.0f, 2.3f);  // Initial position of Pacman
glm::vec3 ghostPositionPink = glm::vec3(0.0f, 0.0f, -0.2f);
glm::vec3 ghostPositionBlue = glm::vec3(-0.5f, 0.0f, -0.2f);
glm::vec3 ghostPositionRed = glm::vec3(0.0f, 0.0f, -1.1f);
glm::vec3 ghostPositionOrange = glm::vec3(0.5f, 0.0f, -0.2f);

const float pacmanSpeed = 2.0f;
float pacmanSpeed_x = 0.0f;
float pacmanSpeed_y = 0.0f;
const float collisionMargin = 0.2f;
float detectionDistance = 0.18f; // Distance to check for collisions

Direction lastDirection = RIGHT; // Initial direction
Direction desiredDirection = RIGHT; // Desired direction

void stopPacman() {
    pacmanSpeed_x = 0.0f;
    pacmanSpeed_y = 0.0f;
}

void handlePacmanControl(int key, int action) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W: // up
            desiredDirection = UP;
            break;
        case GLFW_KEY_S: // down
            desiredDirection = DOWN;
            break;
        case GLFW_KEY_A: // left
            desiredDirection = LEFT;
            break;
        case GLFW_KEY_D: // right
            desiredDirection = RIGHT;
            break;
        }
    }
}

// Check if a point is inside a triangle formed by three vertices
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

void updatePacmanPosition(float deltaTime, const std::vector<float>& mazeVertices) {
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
        switch (desiredDirection) {
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
            // If no collision, update Pacman's position and speed
            pacmanPosition = potentialPosition;
            pacmanSpeed_x = newSpeed_x;
            pacmanSpeed_y = newSpeed_y;
            lastDirection = desiredDirection;
        }
        else {
            // If there's a collision, try to continue in the last valid direction
            potentialPosition = pacmanPosition;
            newSpeed_x = 0.0f;
            newSpeed_y = 0.0f;

            switch (lastDirection) {
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
                pacmanPosition = potentialPosition;
                pacmanSpeed_x = newSpeed_x;
                pacmanSpeed_y = newSpeed_y;
            }
            else {
                // If there's a collision in the last valid direction as well, stop Pacman
                stopPacman();
            }
        }
    }
}
