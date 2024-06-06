#include "model.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include "lodepng.h"
#include "model.h"

bool loadOBJ(const char* filename, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords, float tilingFactor) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Blad: Nie mozna otworzyc pliku: " << filename << std::endl;
        return false;
    }

    // Tymczasowe zmienne do przechowywania danych o modelu
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<Vertex> vertexIndices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") {  // odczytywanie wierzcholkow
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (token == "vn") {  // odczytywanie normalnych
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (token == "vt") {  // odczytywanie wspolrzednych tekstury
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            temp_texCoords.push_back(texCoord);
        }
        else if (token == "f") {  // odczytywanie scian
            std::vector<Vertex> faceVertices;
            std::string vertexString;
            while (iss >> vertexString) {
                Vertex vertexIndex;
                sscanf_s(vertexString.c_str(), "%u/%u/%u", &vertexIndex.v, &vertexIndex.vt, &vertexIndex.vn);
                vertexIndex.v--;
                vertexIndex.vt--;
                vertexIndex.vn--;
                faceVertices.push_back(vertexIndex);
            }

            // Triangulacja scian (z wielokatkow), aby umozliwic ich renderowanie
            for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                vertexIndices.push_back(faceVertices[0]);
                vertexIndices.push_back(faceVertices[i]);
                vertexIndices.push_back(faceVertices[i + 1]);
            }
        }
    }

    // Obliczenie ograniczenia modelu
    glm::vec3 minVertex(FLT_MAX), maxVertex(-FLT_MAX);
    for (const auto& vertex : temp_vertices) {
        minVertex = glm::min(minVertex, vertex);
        maxVertex = glm::max(maxVertex, vertex);
    }

    // Obliczenie srodka modelu
    glm::vec3 center = (minVertex + maxVertex) * 0.5f;

    for (const Vertex& vertexIndex : vertexIndices) {
        // Srodkowanie wierzcho³ka wzglêdem ogrniczaj¹cego bounding box 
        // Zapewnie ustawienie wszystkich modeli, w taki sposób, ¿e srodek modelu jest na srodku sceny
        glm::vec3 vertex = temp_vertices[vertexIndex.v] - center;
        // Dodanie wierzcholkow do modelu
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);

        // Dodanie normalnych do modelu
        if (!temp_normals.empty()) {
            glm::vec3 normal = temp_normals[vertexIndex.vn];
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }

        // Dodanie wspolrzednych tekstur do modelu 
        if (!temp_texCoords.empty()) {
            glm::vec2 texCoord = temp_texCoords[vertexIndex.vt] * tilingFactor; // Skalowanie wspolrzednych tekstury
            texCoords.push_back(texCoord.x);
            texCoords.push_back(texCoord.y);
        }
    }

    file.close();
    return true;
}

Model::Model(const char* objFilename, float tilingFactor) {
    if (!loadOBJ(objFilename, vertices, normals, texCoords, tilingFactor)) {
        fprintf(stderr, "Error: Failed to load OBJ file: %s.\n", objFilename);
        exit(EXIT_FAILURE);
    }

    vertexCount = vertices.size() / 3;
    verticesArray = vertices.data();
    normalsArray = normals.data();
    texCoordsArray = texCoords.data();
}

void Model::draw(ShaderProgram* sp, GLuint texture) {
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, GL_FALSE, 0, verticesArray);

    glEnableVertexAttribArray(sp->a("normal"));
    glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, GL_FALSE, 0, normalsArray);

    glEnableVertexAttribArray(sp->a("texCoord0"));
    glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, GL_FALSE, 0, texCoordsArray);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(sp->u("textureMap0"), 0);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("normal"));
    glDisableVertexAttribArray(sp->a("texCoord0"));
}
