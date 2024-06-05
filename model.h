#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "shaderprogram.h"

class Model {
public:
    std::vector<float> vertices, normals, texCoords;
    GLuint texture;
    int vertexCount;

    Model(const char* objFilename, const char* textureFilename, float tilingFactor);
    void loadTexture(const char* filename);
    void draw(ShaderProgram* sp);

    // Getter for vertices
    const std::vector<float>& getVertices() const { return vertices; }

private:
    float* verticesArray;
    float* normalsArray;
    float* texCoordsArray;
};

bool loadOBJ(const char* filename, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords, float tilingFactor);

#endif 
