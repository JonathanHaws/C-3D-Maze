#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <libs/glm/glm.hpp>
#include <libs/glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <libs/loaders/stb_image.h>
#include "graphics/camera.h"
#include "graphics/window.h"

void clear_gl_errors() {
    while (glGetError() != GL_NO_ERROR);
    }
    
void check_gl_errors() {
    const GLubyte* version = glGetString(GL_VERSION);
    if (version) {
        std::cout << "OpenGL Version: " << version << std::endl;
    } else {
        std::cerr << "Failed to retrieve OpenGL version." << std::endl;
    }
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << err << std::endl;
        }
    }

struct Shader {

    ~Shader() { glDeleteProgram(id); }
    void bind() const { glUseProgram(id); }
    void unbind() const { glUseProgram(0); }
    unsigned int id;
    void setInt(const char* name, int value) const {
        glUniform1i(glGetUniformLocation(id, name), value);
    }
    void setFloat(const char* name, float value) const {
        glUniform1f(glGetUniformLocation(id, name), value);
    }
    void setVec3(const char* name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(id, name), x, y, z);
    }
    void setMat4(const char* name, const glm::mat4& matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    Shader(const std::string& filepath) {
        std::string vertexSource, fragmentSource;
        std::string source = loadShaderSource(filepath);
        parseShaderSource(source, vertexSource, fragmentSource);

        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());
        id = linkShaders(vertexShader, fragmentShader); // Combines and links the shaders into a single shader program
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    void parseShaderSource(const std::string& shaderSource, std::string& vertexSource, std::string& fragmentSource) const {
        size_t vertexPos = shaderSource.find("// Vertex");
        size_t fragmentPos = shaderSource.find("// Fragment");

        if (vertexPos != std::string::npos && fragmentPos != std::string::npos) {
            vertexSource = shaderSource.substr(vertexPos, fragmentPos - vertexPos);
            fragmentSource = shaderSource.substr(fragmentPos);
        } else {
            std::cerr << "Shader section not found in source, should be a '// Vertex' and '// Fragment'" << std::endl;
        }
    }
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

struct Texture {
    int width = 0;
    int height = 0;
    unsigned int textureID = 0;
    const char* imageData = nullptr;

    Texture(int width, int height)
        : width(width), height(height) {
        glGenTextures(1, &textureID);
        resize(width, height);
    }

    Texture(const std::string& filepath, bool mipmaps = true) {
        int channels;
        stbi_set_flip_vertically_on_load(true);
        imageData = reinterpret_cast<const char*>(stbi_load(filepath.c_str(), &width, &height, &channels, 0));
        if (!imageData) {
            std::cerr << "Failed to load texture: " << filepath << std::endl;
            return;
        }
        glGenTextures(1, &textureID);
        updateBuffer(mipmaps);
    }

    ~Texture() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
        stbi_image_free(const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(imageData)));
    }

    void updateTexture(const char* newImageData, int xoffset = 0, int yoffset = 0, int subWidth = -1, int subHeight = -1) {
        if (!newImageData) {
            std::cerr << "Error: New image data is empty." << std::endl;
            return;
        }

        int defaultSubWidth = (subWidth == -1) ? width : subWidth;
        int defaultSubHeight = (subHeight == -1) ? height : subHeight;

        int targetWidth = (defaultSubWidth < width - xoffset) ? defaultSubWidth : (width - xoffset);
        int targetHeight = (defaultSubHeight < height - yoffset) ? defaultSubHeight : (height - yoffset);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, targetWidth, targetHeight, GL_RGB, GL_UNSIGNED_BYTE, newImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void updateBuffer(bool mipmaps = false) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        if (mipmaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void resize(int newWidth, int newHeight) {
        width = newWidth;
        height = newHeight;
        int dataSize = width * height * 3;
        delete[] imageData;
        imageData = new char[dataSize];
        updateBuffer(false);
    }

    void bind(unsigned int texture_unit) const {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void loadFromBitmapFile(const std::string& filepath) {
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
        delete[] imageData;
        imageData = new char[imageSize];
        file.seekg(54);
        file.read(const_cast<char*>(imageData), imageSize);
        if (imageData == nullptr) {
            std::cerr << "Error: Failed to read image data from file: " << filepath << std::endl;
            return;
        }
    }

    void printImageData() {
        std::cout << "Image Data:" << std::endl;
        std::stringstream ss;
        for (size_t i = 0; i < width * height * 3; ++i) {
            ss << static_cast<int>(imageData[i]) << " ";
        }
        std::cout << ss.str() << std::endl;
    }
};

struct Mesh {
    struct Vertex { float x, y, z, u, v, nx, ny, nz; };
    struct Triangle { unsigned int a, b, c; };
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    unsigned int VAO, VBO, EBO;

    Mesh(const std::string& filePath = "") {
        if (!filePath.empty()) {
            if (!loadOBJ(filePath)) { 
                throw std::runtime_error("Failed to load OBJ file: " + filePath); 
            }
        }
        loadBuffers();
    }
    
    ~Mesh() { 
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        }

    void draw(int numInstances = 10) {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        
        if (numInstances == 1) {
            glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, 0);  
        } else {
            glDrawElementsInstanced(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, 0, numInstances);
        }
    }
    
    auto get_flattened_vertices() {
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
    return flattenedVertices;
    }

    auto get_flattened_triangles() {
        std::vector<unsigned int> flattenedTriangles;
        for (const auto& triangle : triangles) {
            flattenedTriangles.push_back(triangle.a);
            flattenedTriangles.push_back(triangle.b);
            flattenedTriangles.push_back(triangle.c);
            }
        return flattenedTriangles;
        }

    void add(const std::vector<Vertex>& new_vertices, const std::vector<Triangle>& new_triangles, float x = 0, float y = 0, float z = 0) {

        for (const auto& new_vertex : new_vertices) {

            vertices.push_back(new_vertex);
            vertices.back().x += x;
            vertices.back().y += y;
            vertices.back().z += z;
            }
            
        int offset = vertices.size() - new_vertices.size();
        for (const auto& new_triangle : new_triangles) {    
                Triangle newTriangle;
                newTriangle.a = new_triangle.a + offset;
                newTriangle.b = new_triangle.b + offset;
                newTriangle.c = new_triangle.c + offset;
                triangles.push_back(newTriangle);
            }
                
        }

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

                Triangle triangle;
                triangle.a = vertices.size() -1;
                triangle.b = vertices.size() - 2;
                triangle.c = vertices.size() - 3;
                triangles.push_back(triangle);
                
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

        updateBuffers();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Specify position floats
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Specify uv coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // Specify in normal vector
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        
        }     

    void updateBuffers() {
        glBindVertexArray(VAO);
        
        std::vector<float> flattenedVertices = get_flattened_vertices();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, flattenedVertices.size() * sizeof(float), flattenedVertices.data(), GL_STATIC_DRAW);

        std::vector<unsigned int> flattenedTriangles = get_flattened_triangles();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, flattenedTriangles.size() * sizeof(unsigned int), flattenedTriangles.data(), GL_STATIC_DRAW);
        }

    };

struct Framebuffer {
    GLuint fbo;
    GLuint color_texture;
    GLuint depth_texture;
    int width;
    int height;
    void bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
    void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    Framebuffer(int width, int height) : width(width), height(height) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &color_texture);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    ~Framebuffer() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &color_texture);
        glDeleteTextures(1, &depth_texture);
    }
};
