#include "game_logic.h"
#include "model.h"
#include <GLFW/glfw3.h>

glm::vec3 mazePosition = glm::vec3(0.0f, -0.3f, 0.0f);
glm::vec3 pacmanPosition = glm::vec3(0.0f, 0.0f, 2.3f);  // Initial position of Pacman
glm::vec3 ghostPositionPink = glm::vec3(0.0f, 0.0f, -0.2f);  
glm::vec3 ghostPositionBlue = glm::vec3(-0.5f, 0.0f, -0.2f);
glm::vec3 ghostPositionRed = glm::vec3(0.0f, 0.0f, -1.1f);   
glm::vec3 ghostPositionOrange = glm::vec3(0.5f, 0.0f, -0.2f);

const float pacmanSpeed = 0.5f;
float pacmanSpeed_x = 0.0f;
float pacmanSpeed_y = 0.0f;

void handlePacmanControl(int key, int action) {
    switch (key) {
    case GLFW_KEY_W: //tyl
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_y = -pacmanSpeed;
        //else if (action == GLFW_RELEASE)
        //    pacmanSpeed_y = 0.0f;
        break;
    case GLFW_KEY_S: //przod
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_y = pacmanSpeed;
        //else if (action == GLFW_RELEASE)
        //    pacmanSpeed_y = 0.0f;
        break;
    case GLFW_KEY_A: //lewo
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_x = -pacmanSpeed;
        //else if (action == GLFW_RELEASE)
        //    pacmanSpeed_x = 0.0f;
        break;
    case GLFW_KEY_D: //prawo
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_x = pacmanSpeed;
        //else if (action == GLFW_RELEASE)
        //    pacmanSpeed_x = 0.0f;
        break;
    }

}

bool checkCollision(const glm::vec3& newPosition, const std::vector<float>& mazeVertices) {
    for (size_t i = 0; i < mazeVertices.size(); i += 9) { // Zak³adamy, ¿e ka¿dy trójk¹t ma 9 floatów (3 wierzcho³ki * 3 wspó³rzêdne)
        glm::vec3 v0(mazeVertices[i], mazeVertices[i + 1], mazeVertices[i + 2]);
        glm::vec3 v1(mazeVertices[i + 3], mazeVertices[i + 4], mazeVertices[i + 5]);
        glm::vec3 v2(mazeVertices[i + 6], mazeVertices[i + 7], mazeVertices[i + 8]);

        // Sprawdzanie kolizji punktu z trójk¹tem
        if (glm::distance(newPosition, v0) < 0.1f || glm::distance(newPosition, v1) < 0.1f || glm::distance(newPosition, v2) < 0.1f) {
            //printf("kolizja");
            return true;
        }
    }
    return false;
}

void updatePacmanPosition(float deltaTime, const std::vector<float>& mazeVertices) {
    glm::vec3 newPosition = pacmanPosition;
    newPosition.x += pacmanSpeed_x * deltaTime;
    newPosition.z += pacmanSpeed_y * deltaTime;
    //printf("kolizje");

    if (!checkCollision(newPosition, mazeVertices)) {
        pacmanPosition = newPosition;
    }
    else {
        pacmanSpeed_x = 0.0f;
        pacmanSpeed_y = 0.0f;
    }
}