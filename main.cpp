#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "lodepng.h"

#include "constants.h"
#include "shaderprogram.h"
#include "model.h"

glm::vec3 pacmanPosition = glm::vec3(0.0f, 0.0f, 0.0f);  // Pacman's initial position
float speed_x = 0.0f;
float speed_y = 0.0f;
const float pacmanSpeed = 0.1f;  // Pacman's movement speed
float moveSpeed = 0.1f; // Prędkość ruchu Pacmana
float aspectRatio = 1.0f; // Stosunek szerokości do wysokości okna

ShaderProgram* sp;

// Models
Model* mazeModel;
Model* pacmanModel;
Model* ghostModel;

//swiatlo
glm::vec4 lightPos1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 lightPos2 = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ks = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

// Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_LEFT) pacmanPosition.x -= moveSpeed;
		if (key == GLFW_KEY_RIGHT) pacmanPosition.x += moveSpeed;
		if (key == GLFW_KEY_UP) pacmanPosition.z -= moveSpeed;
		if (key == GLFW_KEY_DOWN) pacmanPosition.z += moveSpeed;
	}
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
	ghostModel = new Model("resources/INKY.obj", "resources/purple.png");

	glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
	glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
	glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));
}

void freeOpenGLProgram(GLFWwindow* window) {
	delete sp;
	delete mazeModel;
	delete pacmanModel;
	delete ghostModel;
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
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
	pacmanPosition += glm::vec3(speed_x * pacmanSpeed, 0.0f, speed_y * pacmanSpeed);

	// Draw Pacman
	float scale_factor = 0.2f;
	glm::mat4 pacmanM = glm::translate(M, pacmanPosition); // Translate based on Pacman's position
	pacmanM = glm::scale(pacmanM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pacmanM));
	pacmanModel->draw(sp);

	// Draw ghost
	glm::mat4 ghostM = glm::mat4(1.0f);
	ghostM = glm::rotate(ghostM, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
	ghostM = glm::rotate(ghostM, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
	ghostM = glm::scale(ghostM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostM));
	ghostModel->draw(sp);

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
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window)) {
		float deltaTime = glfwGetTime();
		angle_x += speed_x * deltaTime;
		angle_y += speed_y * deltaTime;
		glfwSetTime(0);
		drawScene(window, angle_x, angle_y);
		glfwPollEvents();
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
