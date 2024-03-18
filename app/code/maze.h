#pragma once
#include <vector>
#include <stack>
#include <random>
#include <algorithm> // shuffle random

struct Maze {
    
    int width = 20; // Keep Default bigger then 3 cause it breaks?
    int depth = 20; 
    float height = 0.0f;
    float breadth = 1.0f;
    float timer = 0.0f; 
    float speed = 0.0f;
    bool loop = false; // If true, the maze will reset when it is complete
    float scale = 1.0f;
    float brickSize = 0.5f;

    Camera& camera;
    std::stack<std::pair<int, int>> cells_to_expand; 
    std::vector<std::vector<char>> corridors;
    
    Mesh wall = Mesh("meshes/quad.obj"); 
    Texture mazeTexture = Texture(width, depth);
    Texture bricksDiffuseTexture = Texture("textures/bricks/diffuse.jpg");
    Texture bricksNormalTexture = Texture("textures/bricks/normal.jpg");
    Shader* shader2d;
    Shader* mazeShader;

    Mesh quad;

    Maze(int width, int depth, float height, float breadth, float speed, Camera& camera, bool loop = false, Shader* shader2d = nullptr, Shader* mazeShader = nullptr) :
        width(width), depth(depth), height(height), breadth(breadth), speed(speed), camera(camera), loop(loop), shader2d(shader2d), mazeShader(mazeShader) {
        corridors.resize(width, std::vector<char>(depth, '#'));
        updateTextureFromCorridors();
        }
    char getValue(int x, int y) {
        if (x < 0 || y < 0 || x >= width || y >= depth) { return 'O'; } // Out of bounds 'O
        return corridors[x][y];;
        }
    bool colliding(glm::vec3 position, float radius = 0.2) {
        float x = position.x / scale + (width / 2);
        float y = position.y;
        float z = position.z / scale + (depth / 2);
        
        if (x < 0 - radius || x > (width * scale) + radius) { return false; }
        if (z < 0 - radius || z > (depth * scale) + radius) { return false; }
        if (y < 0 - radius || y > (height * scale) + radius) { return false; }
        
        if (getValue(x + radius, z + radius) == '#') { return true; }
        if (getValue(x - radius, z - radius) == '#') { return true; }
        if (getValue(x + radius, z - radius) == '#') { return true; }
        if (getValue(x - radius, z + radius) == '#') { return true; }

        return false;
        }
 
    void updateTextureFromCorridors(int xoffset = 0, int yoffset = 0, int width = -1, int depth = -1) {
        std::vector<unsigned char> textureData;

        // Determine the range of cells to update based on the provided parameters
        int endX = (width == -1) ? this->width : ((xoffset + width < this->width) ? (xoffset + width) : this->width);
        int endY = (depth == -1) ? this->depth : ((yoffset + depth < this->depth) ? (yoffset + depth) : this->depth);

        for (int y = yoffset; y < endY; ++y) {
            for (int x = xoffset; x < endX; ++x) {
                if (corridors[x][y] == '#') {
                    textureData.push_back(255); 
                    textureData.push_back(255); 
                    textureData.push_back(255); 
                } else {
                    textureData.push_back(0);   
                    textureData.push_back(0);   
                    textureData.push_back(0);  
                }
            }
        }

        // Convert vector to const char*
        const char* imageDataPtr = reinterpret_cast<const char*>(textureData.data());

        // Call the updateTexture function
        mazeTexture.updateTexture(imageDataPtr, xoffset, yoffset, width, depth);
        }

    void reset() {
        corridors.clear();
        corridors.resize(width, std::vector<char>(depth, '#'));
        cells_to_expand = std::stack<std::pair<int, int>>();
        updateTextureFromCorridors();
        }

    void expand_all() { // Instantly expand the entire maze
        while (!cells_to_expand.empty()) {
            expand();
            }
        }
    void expand() {

        if (corridors.size() != width || corridors[0].size() != depth) { // Avoid acessing out of bounds for stack and corridors
            mazeTexture.resize(width, depth);
            reset();
            }

        if (width < 3 || depth < 3) { return; } // Maze is too small to expand (bandaid fix needs to be fixed)
        
        if (cells_to_expand.empty()) { 
            
            if (corridors[1][1] == ' ') { // Maze is complete
                if (loop) { reset(); }
                return;
                } 
            cells_to_expand.push({1, 1}); // Maze is not Started
            }
            

        struct Direction { int dx, dy; };
        std::vector<Direction> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; // North South West East
        std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device()()));

        auto cell = cells_to_expand.top();

        for (const auto& dir : directions) {
            int nx = cell.first + dir.dx * 2;
            int ny = cell.second + dir.dy * 2;
            if (nx >= 0 && ny >= 0 && nx < width && ny < depth ) {
                if (corridors[nx][ny] == '#') {
                    corridors[cell.first + dir.dx][cell.second + dir.dy] = ' ';
                    updateTextureFromCorridors(cell.first + dir.dx, cell.second + dir.dy, 1, 1);
                    corridors[nx][ny] = ' ';
                    updateTextureFromCorridors(nx, ny, 1, 1);
                    cells_to_expand.push({nx, ny});
                    return;
                    }
                }
            }
        
        corridors[cell.first][cell.second] = ' ';
        updateTextureFromCorridors(cell.first, cell.second, 1, 1);
        cells_to_expand.pop();

        }

    void tick(float deltaTime) {
        

        timer += speed * deltaTime;
        while (timer > 1) {
            expand();
            timer --;
            }
        }      

    void drawTexture() {
        shader2d->bind();
        mazeTexture.bind(0);
        glm::mat4 stopclipmodel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f));
        shader2d->setMat4("Model", stopclipmodel);
        shader2d->setMat4("View", glm::mat4(1.0f));
        shader2d->setMat4("Projection", camera.projectionMatrix(false));
        glUniform1i(glGetUniformLocation(shader2d->id, "texture_diffuse1"), 0);
        quad.draw();
        shader2d->setMat4("Projection", camera.projectionMatrix(true));
        shader2d->unbind();
        }

    void draw() {

        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

        glUniform1i(glGetUniformLocation(currentProgram, "mazeWidth"), width);
        glUniform1i(glGetUniformLocation(currentProgram, "mazeDepth"), depth);
        glUniform1f(glGetUniformLocation(currentProgram, "mazeHeight"), height);
        glUniform1f(glGetUniformLocation(currentProgram, "mazeBreadth"), breadth);
        glUniform1f(glGetUniformLocation(currentProgram, "mazeScale"), scale);
        glUniform1f(glGetUniformLocation(currentProgram, "brickSize"), brickSize);

        mazeTexture.bind(0);
        bricksDiffuseTexture.bind(1);
        bricksNormalTexture.bind(2); 
        glUniform1i(glGetUniformLocation(currentProgram, "mazeTexture"), 0);
        glUniform1i(glGetUniformLocation(currentProgram, "bricksDiffuseTexture"), 1);
        glUniform1i(glGetUniformLocation(currentProgram, "bricksNormalTexture"), 2);

        wall.draw(width * depth);

        }
        
    };