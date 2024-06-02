#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <glm/glm.hpp>
#include <vector> // Dodajemy include do std::vector

// Zmienne do przechowywania pozycji 
extern glm::vec3 pacmanPosition, mazePosition, ghostPositionPink, ghostPositionBlue, ghostPositionRed, ghostPositionOrange;

extern float pacmanSpeed_x;
extern float pacmanSpeed_y;

enum Direction { UP, DOWN, LEFT, RIGHT };
extern Direction lastPacmanDirection; // Initial direction
extern Direction desiredPacmanDirection;

void handlePacmanControl(int key, int action);
void updatePacmanPosition(float deltaTime, const std::vector<float>& mazeVertices, bool& gameStarted, bool& gameOver);
void initRandom();
void resetPacman(bool& gameStarted, bool& gameOver);
void resetGhosts();
void updateGhostPositions(float deltaTime, const std::vector<float>& mazeVertices);
bool detectCollision(glm::vec3 position1, glm::vec3 position2, float collisionDistance);
bool checkPacmanGhostCollision();


#endif // GAME_LOGIC_H
