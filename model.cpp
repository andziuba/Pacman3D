#include "model.h"
#include "lodepng.h"
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
        std::cerr << "Error: Cannot open file: " << filename << std::endl;
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

        if (token == "v") { // vertex
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (token == "vn") { // normal
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (token == "vt") { // texture coordinate
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            temp_texCoords.push_back(texCoord);
        }
        else if (token == "f") { // face
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

    // Calculate the bounding box
    glm::vec3 minVertex(FLT_MAX), maxVertex(-FLT_MAX);
    for (const auto& vertex : temp_vertices) {
        minVertex = glm::min(minVertex, vertex);
        maxVertex = glm::max(maxVertex, vertex);
    }

    // Calculate the center of the bounding box
    glm::vec3 center = (minVertex + maxVertex) * 0.5f;

    for (const Vertex& vertexIndex : vertexIndices) {
        glm::vec3 vertex = temp_vertices[vertexIndex.v] - center; // Center the vertex
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

Model::Model(const char* objFilename, const char* textureFilename) {
    if (!loadOBJ(objFilename, vertices, normals, texCoords)) {
        fprintf(stderr, "Error: Failed to load OBJ file: %s.\n", objFilename);
        exit(EXIT_FAILURE);
    }

    vertexCount = vertices.size() / 3;
    verticesArray = vertices.data();
    normalsArray = normals.data();
    texCoordsArray = texCoords.data();

    loadTexture(textureFilename);
}

void Model::loadTexture(const char* filename) {
    glActiveTexture(GL_TEXTURE0);

    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);

    if (error != 0) {
        fprintf(stderr, "Error: Failed to load texture: %s: %s\n", filename, lodepng_error_text(error));
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Model::draw(ShaderProgram* sp) {
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, false, 0, verticesArray);

    glEnableVertexAttribArray(sp->a("normal"));
    glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, false, 0, normalsArray);

    glEnableVertexAttribArray(sp->a("texCoord0"));
    glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoordsArray);

    glUniform1i(sp->u("textureMap0"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("normal"));
    glDisableVertexAttribArray(sp->a("texCoord0"));
}
