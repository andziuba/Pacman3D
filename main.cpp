#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "lodepng.h"
#include "constants.h"
#include "shaderprogram.h"
#include "model.h"
#include "game_logic.h"  // Include the Pacman control header

float cameraSpeed_x = 0.0f;
float cameraSpeed_y = 0.0f;
float aspectRatio = 1.0f;  // Stosunek szerokości do wysokości okna

bool gameStarted = false;

ShaderProgram* sp;

// Modele
Model* mazeModel;
Model* pacmanModel;
Model* ghostModelPink;
Model* ghostModelBlue;
Model* ghostModelRed;
Model* ghostModelOrange;

// Zmienna globalna dla wierzchołków modelu
std::vector<float> mazeVertices;

// światło
glm::vec4 lightPos1 = glm::vec4(5.0f, 10.0f, 5.0f, 1.0f);  // Top-right position
glm::vec4 lightPos2 = glm::vec4(-5.0f, 10.0f, 5.0f, 1.0f); // Top-left position
glm::vec4 ks = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

// Procedura obsługi błędów
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Ruch kamerą za pomocą strzałek
    const float cameraSpeed = PI / 2;
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) cameraSpeed_x = -cameraSpeed;
        if (key == GLFW_KEY_RIGHT) cameraSpeed_x = cameraSpeed;
        if (key == GLFW_KEY_UP) cameraSpeed_y = cameraSpeed;
        if (key == GLFW_KEY_DOWN) cameraSpeed_y = -cameraSpeed;
        if (key == GLFW_KEY_SPACE) {
            gameStarted = true;
            printf("%d", gameStarted);
        }
    }
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) cameraSpeed_x = 0;
        if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) cameraSpeed_y = 0;
    }

    // Pac-Man control logic
    handlePacmanControl(key, action);

}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}

void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    initRandom();

    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");

    mazeModel = new Model("resources/models/labirynt2.obj", "resources/textures/bricks1.png");
    pacmanModel = new Model("resources/models/pacman2.obj", "resources/textures/yellow.png");
    ghostModelPink = new Model("resources/models/duszek2.obj", "resources/textures/pink.png");
    ghostModelBlue = new Model("resources/models/duszek2.obj", "resources/textures/blue.png");
    ghostModelRed = new Model("resources/models/duszek2.obj", "resources/textures/red.png");
    ghostModelOrange = new Model("resources/models/duszek2.obj", "resources/textures/orange.png");

    mazeVertices = mazeModel->getVertices();

    glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
    glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
    glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));
}

void freeOpenGLProgram(GLFWwindow* window) {
    delete sp;
    delete mazeModel;
    delete pacmanModel;
    delete ghostModelPink;
    delete ghostModelBlue;
    delete ghostModelRed;
    delete ghostModelOrange;
    
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y, float deltaTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 V = glm::lookAt(
        glm::vec3(0.0f, 11.0f, 5.0f),
        glm::vec3(0, 0, 0),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f);

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
    M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));

    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

    // Ustawienie wartości uniformów dla źródeł światła i koloru odbić
    glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
    glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
    glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));

    // Draw maze
    glm::mat4 mazeM = glm::translate(M, mazePosition);
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(mazeM));
    mazeModel->draw(sp);

    // Update Pacman's position
    if (gameStarted) {
        updatePacmanPosition(deltaTime, mazeVertices);
        updateGhostPositions(deltaTime, mazeVertices);
    }
    
    // Calculate rotation angle based on Pacman's last movement direction
    float rotationAngle = 0.0f;
    switch (lastPacmanDirection) {
    case RIGHT:
        rotationAngle = 90.0f;
        break;
    case LEFT:
        rotationAngle = -90.0f;
        break;
    case UP:
        rotationAngle = 180.0f;
        break;
    case DOWN:
        rotationAngle = 0.0f;
        break;
    }

    // Draw Pacman
    glm::mat4 pacmanM = glm::translate(M, pacmanPosition); // Translate based on Pacman's position
    pacmanM = glm::rotate(pacmanM, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Apply rotation
    pacmanM = glm::scale(pacmanM, glm::vec3(0.15f, 0.15f, 0.15f)); // Apply scaling
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pacmanM));
    pacmanModel->draw(sp);

    // Draw Ghosts
    glm::vec3 ghostPositions[] = { ghostPositionPink, ghostPositionBlue, ghostPositionRed, ghostPositionOrange };
    Model* ghostModels[] = { ghostModelPink, ghostModelBlue, ghostModelRed, ghostModelOrange };

    for (int i = 0; i < 4; i++) {
        glm::mat4 ghostM = glm::translate(M, ghostPositions[i]);
        ghostM = glm::rotate(ghostM, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ghostM = glm::scale(ghostM, glm::vec3(0.3f, 0.3f, 0.3f)); // Apply scaling
        glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostM));
        ghostModels[i]->draw(sp);
    }

    // Wysłanie do GPU świateł
    glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
    glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
    glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));

    glfwSwapBuffers(window);
}


int main(void) {
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Nie można zainicjować GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(1000, 1000, "Pacman3D", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Nie można utworzyć okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Nie można zainicjować GLEW.\n");
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window);

    float angle_x = 0;
    float angle_y = 0;

    // Główna pętla renderująca
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        static float previousTime = currentTime;
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        angle_x += cameraSpeed_x * deltaTime;
        angle_y += cameraSpeed_y * deltaTime;

        drawScene(window, angle_x, angle_y, deltaTime);

        glfwPollEvents();
    }

    freeOpenGLProgram(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}