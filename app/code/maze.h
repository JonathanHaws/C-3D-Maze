#pragma once
#include <vector>
#include <iostream>
#include <stack>
#include <random>
#include <graphics.h>
#include <algorithm> // shuffle random

struct Maze {
    
    int width = 0; 
    int height = 0; 
    float timer = 0.0f; 
    float speed = 0.0f;
    Camera& camera;
    std::stack<std::pair<int, int>> cells_to_expand; 
    std::vector<std::vector<char>> corridors;
    Mesh wall; 
    Texture texture;
    GLuint instanceBuffer = 0; 
    Shader shader2d;
    Mesh quad;

    Maze(int width, int height, float speed, Camera& camera):
        width(width),
        height(height),
        speed(speed),
        texture(width, height),
        camera(camera),
        wall("meshes/cube.obj"),
        shader2d("shaders/2d.glsl"),
        quad("meshes/quad.obj")
        {
        corridors.resize(width, std::vector<char>(height, '#'));
        updateTextureFromCorridors();
        }
 
    void updateTextureFromCorridors(int xoffset = 0, int yoffset = 0, int width = -1, int height = -1) {
        std::vector<unsigned char> textureData;

        // Determine the range of cells to update based on the provided parameters
        int endX = (width == -1) ? this->width : ((xoffset + width < this->width) ? (xoffset + width) : this->width);
        int endY = (height == -1) ? this->height : ((yoffset + height < this->height) ? (yoffset + height) : this->height);

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
        texture.updateTexture(imageDataPtr, xoffset, yoffset, width, height);
    }

    void reset() {
        corridors.clear();
        corridors.resize(width, std::vector<char>(height, '#'));
        cells_to_expand = std::stack<std::pair<int, int>>();
        updateTextureFromCorridors();
        }

    void expand() {

        if (corridors.size() != width || corridors[0].size() != height) { // Avoid acessing out of bounds for cells_to_expand stack and corridors vector
            texture.resize(width, height);
            reset();
            }

        if(width< 3 || height < 3) { return; } // Maze is too small to expand (bandaid fix somewhere else in this function acessing out of bounds I think)
        
        if (cells_to_expand.empty()) { 
            if (corridors[1][1] == ' ') { return; } //Maze is complete
            cells_to_expand.push({1, 1}); // Maze is not Started
            }

        struct Direction { int dx, dy; };
        std::vector<Direction> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; // North South West East
        std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device()()));

        auto cell = cells_to_expand.top();

        for (const auto& dir : directions) {
            int nx = cell.first + dir.dx * 2;
            int ny = cell.second + dir.dy * 2;
            if (nx >= 0 && ny >= 0 && nx < width && ny < height ) {
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
        shader2d.bind();
        texture.bind(0);
        glm::mat4 stopclipmodel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f));
        shader2d.setMat4("Model", stopclipmodel);
        shader2d.setMat4("View", glm::mat4(1.0f));
        shader2d.setMat4("Projection", camera.projectionMatrix(false));
        glUniform1i(glGetUniformLocation(shader2d.id, "texture_diffuse1"), 0);
        quad.draw();
        shader2d.setMat4("Projection", camera.projectionMatrix(true));
        shader2d.unbind();
    }

    void draw() {

        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        texture.bind(1);

        glUniform1i(glGetUniformLocation(currentProgram, "mazeWidth"), width);
        glUniform1i(glGetUniformLocation(currentProgram, "mazeHeight"), height);
        glUniform1i(glGetUniformLocation(currentProgram, "corridorsTexture"), 1);

        wall.draw(width * height);

    }
        
};