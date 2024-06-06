#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <cstdlib>  
#include <ctime>
#include <stdio.h>
#include <vector>
#include <irrKlang.h>
using namespace irrklang;

#include "lodepng.h"
#include "constants.h"
#include "shaderprogram.h"
#include "model.h"
#include "game_logic.h"
#include "positions.h"

float cameraSpeed_x = 0.0f;
float cameraSpeed_y = 0.0f;
float aspectRatio = 1.0f;  // Stosunek szerokosci do wysokosci okna

// Zmienne globalne statusu gry
extern bool gameStarted = false;
extern bool gameOver = false;

ShaderProgram* sp;

// Modele
Model* mazeModel;
Model* mazeFloorModel;
Model* pacmanModel;
Model* ghostModelRed;
Model* ghostModelBlue;
Model* ghostModelPink;
Model* ghostModelOrange;
Model* pointModel;
Model* logoModel;

// Zmienna globalna dla wierzcholkow modelu labiryntu
std::vector<float> mazeVertices;

// Światło
glm::vec4 lightPos1 = glm::vec4(5.0f, 20.0f, -3.0f, 2.0f);  // Gorne-prawe
glm::vec4 lightPos2 = glm::vec4(-5.0f, 20.0f, -3.0f, 2.0f); // Gorne-lewe
glm::vec4 ks = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

// Procedura obs;ugi bledow
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Ruch kamera za pomoca strzalek
    const float cameraSpeed = PI / 2;
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) cameraSpeed_x = -cameraSpeed;
        if (key == GLFW_KEY_RIGHT) cameraSpeed_x = cameraSpeed;
        if (key == GLFW_KEY_UP) cameraSpeed_y = cameraSpeed;
        if (key == GLFW_KEY_DOWN) cameraSpeed_y = -cameraSpeed;
        if (key == GLFW_KEY_SPACE) {
            gameStarted = true;
            gameOver = false;
            soundEngine->stopAllSounds();
            soundEngine->play2D("resources/audio/pacman_chomp.wav", true);  // Dzwiek rozgrywki
        }
    }
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) cameraSpeed_x = 0;
        if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) cameraSpeed_y = 0;
    }

    // Logika kontroli Pacmanem
    handlePacmanControl(key, action);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}

void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0, 0, 0, 1);  // Ustawienie koloru tla
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    srand(static_cast<unsigned int>(time(nullptr)));  // init random

    // Inicjalizacja programgu shaderow
    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");

    // Ladowanie modeli
    mazeModel = new Model("resources/models/maze1.obj", "resources/textures/walls.png", 6.0f);
    mazeFloorModel = new Model("resources/models/maze_floor.obj", "resources/textures/floor2.png", 1.0f);
    pacmanModel = new Model("resources/models/pacman2.obj", "resources/textures/yellow.png", 1.0f);
    ghostModelRed = new Model("resources/models/duszek2.obj", "resources/textures/red.png", 1.0f);
    ghostModelBlue = new Model("resources/models/duszek2.obj", "resources/textures/blue.png", 1.0f);
    ghostModelPink = new Model("resources/models/duszek2.obj", "resources/textures/pink.png", 1.0f);
    ghostModelOrange = new Model("resources/models/duszek2.obj", "resources/textures/orange.png", 1.0f);
    pointModel = new Model("resources/models/point.obj", "resources/textures/gold.png", 1.0f);
    logoModel = new Model("resources/models/logo.obj", "resources/textures/yellow.png", 3.0f);

    // Pobranie wierzchołków modelu labiryntu
    mazeVertices = mazeModel->getVertices();

    // Ustawienie pozycji swiatel
    glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
    glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
    glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));

    // Inicjalizacja silnika dzwieku
    soundEngine = createIrrKlangDevice();
    if (!soundEngine) {
        fprintf(stderr, "Could not initialize IrrKlang engine.\n");
        exit(EXIT_FAILURE);
    }

    system("cls");
    printf("Controls:\nWASD keys - Pacman movement\nArrow keys - camera movement\n\n");
    printf("Press space to start");
    soundEngine->play2D("resources/audio/pacman_beginning.wav", true);  // Dzwięk początkowy
}

// Zwolenienie zasobow
void freeOpenGLProgram(GLFWwindow* window) {
    delete sp;
    delete mazeModel;
    delete mazeFloorModel;
    delete pacmanModel;
    delete ghostModelRed;
    delete ghostModelBlue;
    delete ghostModelPink;
    delete ghostModelOrange;
    delete pointModel;
    delete logoModel;
}

void drawPoint(const glm::mat4& baseMatrix, const glm::vec3& position, ShaderProgram* shaderProgram, Model* model) {
    glm::mat4 pointMatrix = glm::translate(baseMatrix, position);
    glUniformMatrix4fv(shaderProgram->u("M"), 1, false, glm::value_ptr(pointMatrix));
    model->draw(shaderProgram);
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y, float deltaTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Wyczysc bufor koloru i głębokości

    // Macierz widoku
    glm::mat4 V = glm::lookAt(
        glm::vec3(0.0f, 11.0f, 5.0f),
        glm::vec3(0, 0, 0),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Macierz pozycji
    glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f);

    // Macierz modelu
    glm::mat4 M = glm::mat4(1.0f);
    M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
    M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));

    sp->use();  // Aktywacja programu shaderow
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

    // Ustawienie wartosci uniformow dla zrodel swiatla i koloru odbic
    glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
    glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
    glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));

    // Rysowanie loga
    glm::mat4 logoM = glm::translate(M, logoPosition);
    logoM = glm::rotate(logoM, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    logoM = glm::scale(logoM, glm::vec3(0.2f, 0.2f, 0.2f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(logoM));
    logoModel->draw(sp);

    // Rysowanie labiryntu
    glm::mat4 mazeM = glm::translate(M, mazePosition);
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(mazeM));
    mazeModel->draw(sp);

    // Rysowanie podlogi
    glm::mat4 mazeFloorM = glm::translate(M, mazeFloorPosition);
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(mazeFloorM));
    mazeFloorModel->draw(sp);

    // Zmiana pozycji Pacmana i duszkow, jeśli gra jest rozpoczeta i nie zakonczona
    if (gameStarted && !gameOver) {
        updatePacmanPosition(deltaTime, mazeVertices, gameStarted, gameOver);
        updateGhostPositions(deltaTime, mazeVertices);
    }

    // Rotacja Pacmana
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

    // Rysowanie monet
    for (const auto& position : pointPositions) {
        drawPoint(M, position, sp, pointModel);
    }

    // Rysowanie Pacmana
    glm::mat4 pacmanM = glm::translate(M, pacmanPosition);
    pacmanM = glm::rotate(pacmanM, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    pacmanM = glm::scale(pacmanM, glm::vec3(0.15f, 0.15f, 0.15f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pacmanM));
    pacmanModel->draw(sp);

    // Rysowanie duszkow
    glm::vec3 ghostPositions[] = { ghostPositionRed, ghostPositionBlue, ghostPositionPink, ghostPositionOrange };
    Model* ghostModels[] = { ghostModelRed, ghostModelBlue, ghostModelPink, ghostModelOrange };

    for (int i = 0; i < 4; i++) {
        glm::mat4 ghostM = glm::translate(M, ghostPositions[i]);
        ghostM = glm::rotate(ghostM, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ghostM = glm::scale(ghostM, glm::vec3(0.3f, 0.3f, 0.3f));
        glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostM));
        ghostModels[i]->draw(sp);
    }

    // Wyslanie do GPU swiatel
    glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
    glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
    glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));

    glfwSwapBuffers(window);  // Zmiana bufora - wyswietlanie nowej sceny
}

int main(void) {
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Nie mozna zainicjowac GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(1000, 1000, "Pacman3D", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Nie mozna utworzyć okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Nie można zainicjowac GLEW.\n");
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window);

    float angle_x = 0;
    float angle_y = 0;

    // Glowna petla renderująca
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
