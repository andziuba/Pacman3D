#include "game_logic.h"
#include <GLFW/glfw3.h>

glm::vec3 pacmanPosition = glm::vec3(0.0f, 0.0f, 0.8f);  // Initial position of Pacman
const float pacmanSpeed = 0.1f;
float pacmanSpeed_x = 0.0f;
float pacmanSpeed_y = 0.0f;

void handlePacmanControl(int key, int action) {
   // const float pacmanSpeed = 0.1f;
    switch (key) {
    case GLFW_KEY_W:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_y = pacmanSpeed;
        else
            pacmanSpeed_y = 0.0f;
        break;
    case GLFW_KEY_S:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_y = -pacmanSpeed;
        else
            pacmanSpeed_y = 0.0f;
        break;
    case GLFW_KEY_A:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_x = -pacmanSpeed;
        else
            pacmanSpeed_x = 0.0f;
        break;
    case GLFW_KEY_D:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            pacmanSpeed_x = pacmanSpeed;
        else
            pacmanSpeed_x = 0.0f;
        break;
    }


}
