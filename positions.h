#ifndef POSITIONS_H
#define POSITIONS_H

#include <glm/glm.hpp>
#include <vector>

extern glm::vec3 mazePosition;
extern glm::vec3 mazeFloorPosition;

extern glm::vec3 logoPosition;

// Miejsce na mapie, do ktorego nie maj¹ dostepu duszki, po wyjsciu z niego
extern const glm::vec3 restrictedZoneMin;
extern const glm::vec3 restrictedZoneMax;

extern glm::vec3 pacmanPositionInitial;

extern glm::vec3 ghostPositionRedInitial;
extern glm::vec3 ghostPositionBlueInitial;
extern glm::vec3 ghostPositionPinkInitial;
extern glm::vec3 ghostPositionOrangeInitial;

extern std::vector<glm::vec3> pointPositions;
extern std::vector<glm::vec3> pointPositionsCopy;

#endif
