#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <glm/glm.hpp>
#include <vector> // Dodajemy include do std::vector

// Zmienna do przechowywania pozycji Pac-Mana
extern glm::vec3 pacmanPosition;
extern float pacmanSpeed_x;
extern float pacmanSpeed_y;

void handlePacmanControl(int key, int action);
void updatePacmanPosition(float deltaTime, const std::vector<float>& mazeVertices);
bool checkCollision(const glm::vec3& newPosition, const std::vector<float>& mazeVertices);

#endif // GAME_LOGIC_H
