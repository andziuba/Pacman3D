#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <vector>
#include <glm/glm.hpp>

bool loadOBJ(const char* filename, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords);

#endif 
