#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Mesh {
    public:
        Mesh(const std::string& filePath) {
            if (!loadOBJ(filePath)) {
                throw std::runtime_error("Failed to load OBJ file: " + filePath);
                }
            loadBuffers();
            }   
        ~Mesh() { 
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            }
        unsigned int getVAO() const { return VAO; }
        unsigned int getVBO() const { return VBO; }
        unsigned int getEBO() const { return EBO; }
        int getVertexCount() const { return vertices.size(); }

    private:
        bool loadOBJ(const std::string& filePath) { // TODO: make more efficent by removing duplicate vertices
            std::ifstream file(filePath);
            if (!file.is_open()) { throw std::runtime_error("Failed to open OBJ file: " + filePath);}

            struct TempVertex { float x, y, z; };
            struct TempUV { float u, v; };
            struct TempNormal { float nx, ny, nz; };
            std::vector<TempVertex> tempVertices;
            std::vector<TempUV> tempUVs;
            std::vector<TempNormal> tempNormals;

            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string type;
                iss >> type;
                if (type == "v") {
                    TempVertex tempVertex;
                    iss >> tempVertex.x >> tempVertex.y >> tempVertex.z;
                    tempVertices.push_back(tempVertex);
                } else if (type == "vt") {
                    TempUV tempUV;
                    iss >> tempUV.u >> tempUV.v;
                    tempUVs.push_back(tempUV);
                } else if (type == "vn") {
                    TempNormal tempNormal;
                    iss >> tempNormal.nx >> tempNormal.ny >> tempNormal.nz;
                    tempNormals.push_back(tempNormal);
                } else if (type == "f") {
                    char slash;
                    Vertex vertex;
                    
                    for (int i = 0; i < 3; ++i) {
                        unsigned int vertexIndex, uvIndex, normalIndex;
                        iss >> vertexIndex >> slash >> uvIndex >> slash >> normalIndex;
                        vertex.x = tempVertices[vertexIndex - 1].x;
                        vertex.y = tempVertices[vertexIndex - 1].y;
                        vertex.z = tempVertices[vertexIndex - 1].z;
                        vertex.u = tempUVs[uvIndex - 1].u;
                        vertex.v = tempUVs[uvIndex - 1].v;
                        vertex.nx = tempNormals[normalIndex - 1].nx;
                        vertex.ny = tempNormals[normalIndex - 1].ny;
                        vertex.nz = tempNormals[normalIndex - 1].nz;
                        vertices.push_back(vertex);
                        }

                    Index index;
                    index.v3 = vertices.size() -1;
                    index.v2 = vertices.size() - 2;
                    index.v1 = vertices.size() - 3;
                    indices.push_back(index);
                    
                    }
                }

            #pragma region debug output
                // for (const auto& vertex : vertices) {
                //     std::cout << "Vertex: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << "), "
                //             << "UV: (" << vertex.u << ", " << vertex.v << "), "
                //             << "Normal: (" << vertex.nx << ", " << vertex.ny << ", " << vertex.nz << ")" << std::endl;
                //     }

                // std::cout << "Indices: " << std::endl;
                //     for (size_t i = 0; i < indices.size(); ++i) {
                //         std::cout << "    (" << indices[i].v1 << ", " << indices[i].v2 << ", " << indices[i].v3 << ")" << std::endl;
                //         }
                    #pragma endregion

            return true;
            }

        void loadBuffers() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            std::vector<float> flattenedVertices;
            for (const auto& vertex : vertices) {
                flattenedVertices.push_back(vertex.x);
                flattenedVertices.push_back(vertex.y);
                flattenedVertices.push_back(vertex.z);
                flattenedVertices.push_back(vertex.u);
                flattenedVertices.push_back(vertex.v);
                flattenedVertices.push_back(vertex.nx);
                flattenedVertices.push_back(vertex.ny);
                flattenedVertices.push_back(vertex.nz);
                }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, flattenedVertices.size() * sizeof(float), flattenedVertices.data(), GL_STATIC_DRAW);

            std::vector<unsigned int> flattenedIndices;
            for (const auto& index : indices) {
                flattenedIndices.push_back(index.v1);
                flattenedIndices.push_back(index.v2);
                flattenedIndices.push_back(index.v3);
                }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, flattenedIndices.size() * sizeof(unsigned int), flattenedIndices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Specify position floats
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Specify uv coordinates
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // Specify in normal vector
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
            
            }

        struct Vertex {
            float x, y, z, u, v, nx, ny, nz;
            };
        struct Index {
            unsigned int v1, v2, v3;
            };
        std::vector<Vertex> vertices;
        std::vector<Index> indices;
        unsigned int VAO, VBO, EBO;
    };


