#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <glm/glm.hpp>
#include <vector>

extern glm::vec3 pacmanPosition;
extern const float pacmanSpeed;
extern float pacmanSpeed_x;
extern float pacmanSpeed_y;

void handlePacmanControl(int key, int action);

#endif
