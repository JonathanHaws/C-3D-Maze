
#pragma once

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