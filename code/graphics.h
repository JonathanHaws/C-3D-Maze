#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath) {
            unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, loadShaderSource(vertexPath));
            unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, loadShaderSource(fragmentPath));
            id = linkShaders(vertexShader, fragmentShader); // Combines and links the shaders into a single shader program
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            }

        ~Shader() { 
            glDeleteProgram(id); 
            }

        void bind() const {
            glUseProgram(id);
            }

        void unbind() const {
            glUseProgram(0);
            }

        unsigned int getID() const {
            return id;
            }
    private:
        unsigned int id;
        const char* loadShaderSource(const std::string& filepath) const {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open shader file: " << filepath << std::endl;
                return nullptr;
                }

            std::stringstream buffer;
            buffer << file.rdbuf();

            std::string sourceString = buffer.str();
            char* source = new char[sourceString.size() + 1];
            std::copy(sourceString.begin(), sourceString.end(), source);
            source[sourceString.size()] = '\0'; // Null-terminate the string

            return source;
            }

        unsigned int compileShader(unsigned int type, const char* source) const {
            unsigned int shaderID = glCreateShader(type);
            glShaderSource(shaderID, 1, &source, nullptr);
            glCompileShader(shaderID);

            int success;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
                std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
                }

            return shaderID;
            }

        unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader) const {
            unsigned int programID = glCreateProgram();
            glAttachShader(programID, vertexShader);
            glAttachShader(programID, fragmentShader);
            glLinkProgram(programID);

            int success;
            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(programID, 512, nullptr, infoLog);
                std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
            }

            return programID;
        }
    };

class Texture {
    
    private:
        
        int width = 0; 
        int height = 0;
        unsigned int textureID = 0; // Texture ID
        std::vector<unsigned char> imageData;
    
    public:
        Texture(int width, int height, GLenum component) { // generate texture from default frame buffer
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
        }


        Texture(const std::string& filepath) {
            // Load image data from bitmap file
            std::ifstream file(filepath, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Error: Unable to open file: " << filepath << std::endl;
                return;
                }
            file.seekg(18);
            file.read(reinterpret_cast<char*>(&width), sizeof(width));
            file.read(reinterpret_cast<char*>(&height), sizeof(height));
            if (width <= 0 || height <= 0) {
                std::cerr << "Error: Invalid image dimensions in file: " << filepath << std::endl;
                return;
                }
            size_t imageSize = width * height * 3;
            imageData.resize(imageSize);
            file.seekg(54);
            file.read(reinterpret_cast<char*>(imageData.data()), imageSize);
            if (imageData.empty()) {
                std::cerr << "Error: Failed to read image data from file: " << filepath << std::endl;
                return;
                }

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData.data());
            glGenerateMipmap(GL_TEXTURE_2D);
            }

        ~Texture() {
            if (textureID != 0) {
                glDeleteTextures(1, &textureID);
                textureID = 0;
                }
            }

        void printImageData() const {
            std::cout << "Image Data:" << std::endl;
            for (size_t i = 0; i < imageData.size(); ++i) {
                std::cout << static_cast<int>(imageData[i]) << " ";
                }
            std::cout << std::endl;
            }

        void genBuffer() {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData.data());
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        void bind(int textureUnit) const {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);
            }

        int getWidth() const { return width; }
        int getHeight() const { return height; }
        unsigned int getID() const { return textureID; }
        const std::vector<unsigned char>& getImageData() const { return imageData; }
    };

class Mesh {
    
    public:
        
        Mesh(const std::string& filePath) : position(0.0f), rotation(0.0f), scale(1.0f) {
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

        void draw() {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            // glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) *
            //     glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
            //     glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
            //     glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
            //     glm::scale(glm::mat4(1.0f), scale);
            //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
                
            glDrawElements(GL_TRIANGLES, getVertexCount(), GL_UNSIGNED_INT, 0);
            }

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
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::mat4 modelMatrix;
        
    };

class Camera {

    public:
        Camera(const glm::vec3& initialPosition = glm::vec3(0.0f, 20.0f, -40.0f),
                const glm::vec3& initialTarget = glm::vec3(0.0f, 0.0f, -20.0f),
                const glm::vec3& initialUp = glm::vec3(0.0f, 1.0f, 0.0f),
                float fov = 80.0f,
                float aspectRatio = 1280.0f / 720.0f,
                float nearPlane = 0.1f,
                float farPlane = 1000.0f)
                : position(initialPosition), target(initialTarget), up(initialUp), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {}
                
        void draw(Mesh& mesh, float x, float y, float z, Texture& texture, unsigned int shaderProgram) const {

            glUseProgram(shaderProgram);
    
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Model"), 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "View"), 1, GL_FALSE, glm::value_ptr(glm::lookAt(position, target, up)));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Projection"), 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane)));

            texture.bind(0);
            mesh.draw();
            }
        
        void draw2d(Mesh& mesh, float x, float y, float z, Texture& texture, unsigned int shaderProgramtousetodraw) const {

            glUseProgram(shaderProgramtousetodraw);
            texture.bind(0);
            mesh.draw();
            }

        float get_pitch() const {
            glm::vec3 direction = glm::normalize(target - position);
            return glm::degrees(asin(direction.y));
            }

        float get_yaw() const {
            glm::vec3 direction = glm::normalize(target - position);
            float yaw = atan2(direction.z, direction.x);
            if (yaw < 0) {
                yaw += 2 * glm::pi<float>(); // Ensure yaw is positive
            }
            return glm::degrees(yaw);
            }

        void setAspectRatio(float newAspectRatio) {
            aspectRatio = newAspectRatio;
            }

        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;
        };
