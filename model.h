#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "lodepng.h"
#include "shaderprogram.h"

// Struktura do przechowywania indeksow wierzcholkow dla pozycji (v), tekstury (vt), normalnych (vn)
struct Vertex {
    unsigned int v, vt, vn;
};

bool loadOBJ(const char* filename, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords, float tilingFactor);

class Model {
public:
    std::vector<float> vertices;  // Wierzcholki
    std::vector<float> normals;   // Normalne
    std::vector<float> texCoords; // Wspolrzêdne tekstury
    GLuint texture;               // Tekstura modelu
    int vertexCount;              // Liczba wierzcholkow

    Model(const char* objFilename, float tilingFactor);
    void draw(ShaderProgram* sp, GLuint texture);

    // Getter dla wierzcholkow
    const std::vector<float>& getVertices() const { return vertices; }

private:
    float* verticesArray;
    float* normalsArray;
    float* texCoordsArray;
};

#endif 
