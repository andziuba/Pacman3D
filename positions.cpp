#include "positions.h"

glm::vec3 mazePosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 mazeFloorPosition = glm::vec3(0.0f, -0.5f, 0.0f);

// Miejsce na mapie, do którego nie maj¹ dostêpu duszki, po wyjœciu z niego
const glm::vec3 restrictedZoneMin(-1.0f, 0.0f, -1.0f);
const glm::vec3 restrictedZoneMax(1.0f, 0.0f, 0.3f);

glm::vec3 pacmanPositionInitial = glm::vec3(0.0f, 0.0f, 2.5f);

glm::vec3 ghostPositionRedInitial = glm::vec3(0.0f, 0.0f, -1.1f);
glm::vec3 ghostPositionBlueInitial = glm::vec3(-0.5f, 0.0f, -0.2f);
glm::vec3 ghostPositionPinkInitial = glm::vec3(0.0f, 0.0f, -0.2f);
glm::vec3 ghostPositionOrangeInitial = glm::vec3(0.5f, 0.0f, -0.2f);

std::vector<glm::vec3> pointPositions = {
    glm::vec3(3.8f, 0.0f, 4.3f),
    glm::vec3(2.85f, 0.0f, 4.3f),
    glm::vec3(1.9f, 0.0f, 4.3f),
    glm::vec3(0.95f, 0.0f, 4.3f),
    glm::vec3(0.0f, 0.0f, 4.3f),
    glm::vec3(-0.95f, 0.0f, 4.3f),
    glm::vec3(-1.9f, 0.0f, 4.3f),
    glm::vec3(-2.85f, 0.0f, 4.3f),
    glm::vec3(-3.8f, 0.0f, 4.3f),

    glm::vec3(3.8f, 0.0f, 3.4f),
    glm::vec3(2.85f, 0.0f, 3.4f),
    glm::vec3(0.95f, 0.0f, 3.4f),
    glm::vec3(-3.8f, 0.0f, 3.4f),
    glm::vec3(-2.85f, 0.0f, 3.4f),
    glm::vec3(-0.95f, 0.0f, 3.4f),

    glm::vec3(3.8f, 0.0f, 2.45f),
    glm::vec3(1.9f, 0.0f, 2.45f),
    glm::vec3(0.95f, 0.0f, 2.45f),
    glm::vec3(-0.95f, 0.0f, 2.45f),
    glm::vec3(-1.9f, 0.0f, 2.45f),
    glm::vec3(-3.8f, 0.0f, 2.45f),

    glm::vec3(3.8f, 0.0f, 1.5f),
    glm::vec3(2.85f, 0.0f, 1.5f),
    glm::vec3(1.9f, 0.0f, 1.5f),
    glm::vec3(0.95f, 0.0f, 1.5f),
    glm::vec3(-0.95f, 0.0f, 1.5f),
    glm::vec3(-1.9f, 0.0f, 1.5f),
    glm::vec3(-2.85f, 0.0f, 1.5f),
    glm::vec3(-3.8f, 0.0f, 1.5f),

    glm::vec3(0.95f, 0.0f, 0.6f),
    glm::vec3(0.0f, 0.0f, 0.6f),
    glm::vec3(-0.95f, 0.0f, 0.6f),

    glm::vec3(3.8f, 0.0f, -0.3f),
    glm::vec3(2.85f, 0.0f, -0.3f),
    glm::vec3(1.9f, 0.0f, -0.3f),
    glm::vec3(-1.9f, 0.0f, -0.3f),
    glm::vec3(-2.85f, 0.0f, -0.3f),
    glm::vec3(-3.8f, 0.0f, -0.3f),

    glm::vec3(0.95f, 0.0f, -1.2f),
    glm::vec3(0.0f, 0.0f, -1.2f),
    glm::vec3(-0.95f, 0.0f, -1.2f),

    glm::vec3(3.8f, 0.0f, -2.15f),
    glm::vec3(2.85f, 0.0f, -2.15f),
    glm::vec3(0.95f, 0.0f, -2.15f),
    glm::vec3(-0.95f, 0.0f, -2.15f),
    glm::vec3(-2.85f, 0.0f, -2.15f),
    glm::vec3(-3.8f, 0.0f, -2.15f),

    glm::vec3(3.8f, 0.0f, -3.05f),
    glm::vec3(2.85f, 0.0f, -3.05f),
    glm::vec3(1.9f, 0.0f, -3.05f),
    glm::vec3(0.95f, 0.0f, -3.05f),
    glm::vec3(0.0f, 0.0f, -3.05f),
    glm::vec3(-0.95f, 0.0f, -3.05f),
    glm::vec3(-1.9f, 0.0f, -3.05f),
    glm::vec3(-2.85f, 0.0f, -3.05f),
    glm::vec3(-3.8f, 0.0f, -3.05f),

    glm::vec3(3.8f, 0.0f, -4.3f),
    glm::vec3(2.85f, 0.0f, -4.3f),
    glm::vec3(1.9f, 0.0f, -4.3f),
    glm::vec3(0.95f, 0.0f, -4.3f),
    glm::vec3(-0.95f, 0.0f, -4.3f),
    glm::vec3(-1.9f, 0.0f, -4.3f),
    glm::vec3(-2.85f, 0.0f, -4.3f),
    glm::vec3(-3.8f, 0.0f, -4.3f),
};
