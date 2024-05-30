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

ShaderProgram* sp;

// Modele
Model* mazeModel;
Model* pacmanModel;
Model* ghostModelPurple;
Model* ghostModelBlue;
Model* ghostModelRed;
Model* ghostModelOrange;

// światło
glm::vec4 lightPos1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 lightPos2 = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ks = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

// Zmienne globalne
std::vector<float> mazeVertices;
std::vector<float> normals; // Dodajemy zmienne normals i texCoords
std::vector<float> texCoords;

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

    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");

    mazeModel = new Model("resources/maze.obj", "resources/bricks1.png");
    pacmanModel = new Model("resources/pacman.obj", "resources/s_pme_a0_cmp4.png");
    ghostModelPurple = new Model("resources/INKY.obj", "resources/purple.png");
    ghostModelBlue = new Model("resources/INKY.obj", "resources/blue.png");
    ghostModelRed = new Model("resources/INKY.obj", "resources/red.png");
    ghostModelOrange = new Model("resources/INKY.obj", "resources/orange.png");

    if (!loadOBJ("resources/maze.obj", mazeVertices, normals, texCoords)) {
        fprintf(stderr, "Blad: Nie udalo sie wczytac pliku OBJ: resources/maze.obj.\n");
        exit(EXIT_FAILURE);
    }

    glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
    glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
    glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));
}

void freeOpenGLProgram(GLFWwindow* window) {
    delete sp;
    delete mazeModel;
    delete pacmanModel;
    delete ghostModelPurple;
    delete ghostModelBlue;
    delete ghostModelRed;
    delete ghostModelOrange;
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y, float deltaTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 V = glm::lookAt(
        glm::vec3(0, 4.5, 3),
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
    mazeModel->draw(sp);

    // Update Pacman's position
    updatePacmanPosition(deltaTime, mazeVertices);

    // Draw Pacman
    float scale_factor = 0.2f;
    glm::mat4 pacmanM = glm::translate(M, pacmanPosition); // Translate based on Pacman's position
    pacmanM = glm::scale(pacmanM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pacmanM));
    pacmanModel->draw(sp);

    // Draw ghosts
    glm::mat4 ghostPurpleM = glm::mat4(1.0f);
    ghostPurpleM = glm::rotate(ghostPurpleM, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
    ghostPurpleM = glm::rotate(ghostPurpleM, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
    ghostPurpleM = glm::scale(ghostPurpleM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostPurpleM));
    ghostModelPurple->draw(sp);

    glm::mat4 ghostOrangeM = glm::mat4(1.0f);
    ghostOrangeM = glm::rotate(ghostOrangeM, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
    ghostOrangeM = glm::rotate(ghostOrangeM, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
    ghostOrangeM = glm::scale(ghostOrangeM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostOrangeM));
    ghostModelOrange->draw(sp);

    glm::mat4 ghostBlueM = glm::mat4(1.0f);
    ghostBlueM = glm::rotate(ghostBlueM, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
    ghostBlueM = glm::rotate(ghostBlueM, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
    ghostBlueM = glm::scale(ghostBlueM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostBlueM));
    ghostModelBlue->draw(sp);

    glm::mat4 ghostRedM = glm::mat4(1.0f);
    ghostRedM = glm::rotate(ghostRedM, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
    ghostRedM = glm::rotate(ghostRedM, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
    ghostRedM = glm::scale(ghostRedM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostRedM));
    ghostModelRed->draw(sp);

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