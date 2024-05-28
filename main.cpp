#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "shaderprogram.h"
#include "obj_loader.h"
#include "lodepng.h"

glm::vec3 pacmanPosition = glm::vec3(0.0f, 0.0f, 0.0f);  // Pacman's initial position
float speed_x = 0.0f;
float speed_y = 0.0f;
const float pacmanSpeed = 0.1f;  // Pacman's movement speed
float moveSpeed = 0.1f; // Prędkość ruchu Pacmana
float aspectRatio = 1.0f;

ShaderProgram* sp;

// Labirynt
std::vector<float> mazeVertices, mazeNormals, mazeTexCoords;
float* maze_vertices;
float* maze_normals;
float* maze_texCoords;
int maze_vertexCount;

// Pacman
std::vector<float> pacmanVertices, pacmanNormals, pacmanTexCoords;
float* pacman_vertices;
float* pacman_normals;
float* pacman_texCoords;
int pacman_vertexCount;

// Duszek
std::vector<float> ghostVertices, ghostNormals, ghostTexCoords;
float* ghost_vertices;
float* ghost_normals;
float* ghost_texCoords;
int ghost_vertexCount;

// Tekstury
GLuint mazeTex;
GLuint pacmanTex1;
GLuint ghostTex1;

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
		if (key == GLFW_KEY_LEFT) {
			speed_x = -1.0f;
			speed_y = 0.0f;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_x = 1.0f;
			speed_y = 0.0f;
		}
		if (key == GLFW_KEY_UP) {
			speed_x = 0.0f;
			speed_y = -1.0f;
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = 0.0f;
			speed_y = 1.0f;
		}
	}
}


void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");

	if (!loadOBJ("resources/maze.obj", mazeVertices, mazeNormals, mazeTexCoords)) {
		fprintf(stderr, "Nie udało się wczytać pliku OBJ dla labiryntu.\n");
		exit(EXIT_FAILURE);
	}

	maze_vertices = mazeVertices.data();
	maze_normals = mazeNormals.data();
	maze_texCoords = mazeTexCoords.data();
	maze_vertexCount = mazeVertices.size() / 3;

	if (!loadOBJ("resources/pacman.obj", pacmanVertices, pacmanNormals, pacmanTexCoords)) {
		fprintf(stderr, "Nie udało się wczytać pliku OBJ dla Pacmana.\n");
		exit(EXIT_FAILURE);
	}

	pacman_vertices = pacmanVertices.data();
	pacman_normals = pacmanNormals.data();
	pacman_texCoords = pacmanTexCoords.data();
	pacman_vertexCount = pacmanVertices.size() / 3;

	if (!loadOBJ("resources/INKY.obj", ghostVertices, ghostNormals, ghostTexCoords)) {
		fprintf(stderr, "Nie udało się wczytać pliku OBJ dla Pacmana.\n");
		exit(EXIT_FAILURE);
	}

	ghost_vertices = ghostVertices.data();
	ghost_normals = ghostNormals.data();
	ghost_texCoords = ghostTexCoords.data();
	ghost_vertexCount = ghostVertices.size() / 3;

	mazeTex = readTexture("resources/bricks1.png");
	pacmanTex1 = readTexture("resources/s_pme_a0_cmp4.png");
	ghostTex1 = readTexture("resources/purple.png");

	// Ustawienie wartości uniformów dla źródeł światła i koloru odbić
	glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lightPos1));
	glUniform4fv(sp->u("lp2"), 1, glm::value_ptr(lightPos2));
	glUniform4fv(sp->u("ks"), 1, glm::value_ptr(ks));
}


void freeOpenGLProgram(GLFWwindow* window) {
	delete sp;
	glDeleteTextures(1, &mazeTex);
	glDeleteTextures(1, &pacmanTex1);
	glDeleteTextures(1, &ghostTex1);
}

void drawObject(float* vertices, float* normals, float* texCoords, int vertexCount, GLuint texture) {
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, false, 0, vertices);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, false, 0, normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
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
	drawObject(maze_vertices, maze_normals, maze_texCoords, maze_vertexCount, mazeTex);

	// Update Pacman's position
	pacmanPosition += glm::vec3(speed_x * pacmanSpeed, 0.0f, speed_y * pacmanSpeed);

	// Draw Pacman
	float scale_factor = 0.2f;

	glm::mat4 pacmanM = glm::translate(M, pacmanPosition); // Translate based on Pacman's position
	pacmanM = glm::scale(pacmanM, glm::vec3(scale_factor, scale_factor, scale_factor)); // Apply scaling

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pacmanM));

	drawObject(pacman_vertices, pacman_normals, pacman_texCoords, pacman_vertexCount, pacmanTex1);

	// Draw ghost
	glm::mat4 ghostM = glm::mat4(1.0f);
	ghostM = glm::rotate(ghostM, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
	ghostM = glm::rotate(ghostM, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(ghostM));

	drawObject(ghost_vertices, ghost_normals, ghost_texCoords, ghost_vertexCount, ghostTex1);

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
