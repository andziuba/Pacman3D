#include "obj_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Vertex {
    unsigned int v, vt, vn;
};

bool loadOBJ(const char* filename, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "B³¹d: nie uda³o siê wczytaæ pliku " << filename << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<Vertex> vertexIndices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") { // Vertex
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (token == "vn") { // Normal
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (token == "vt") { // Texture coordinate
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            temp_texCoords.push_back(texCoord);
        }
        else if (token == "f") { // Face
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
            // Triangulate the face (assumes a convex polygon)
            for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                vertexIndices.push_back(faceVertices[0]);
                vertexIndices.push_back(faceVertices[i]);
                vertexIndices.push_back(faceVertices[i + 1]);
            }
        }
    }

    for (const Vertex& vertexIndex : vertexIndices) {
        glm::vec3 vertex = temp_vertices[vertexIndex.v];
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);

        if (!temp_normals.empty()) {
            glm::vec3 normal = temp_normals[vertexIndex.vn];
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }

        if (!temp_texCoords.empty()) {
            glm::vec2 texCoord = temp_texCoords[vertexIndex.vt];
            texCoords.push_back(texCoord.x);
            texCoords.push_back(texCoord.y);
        }
    }

    file.close();
    return true;
}
