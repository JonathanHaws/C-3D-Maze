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
    float expandTimer = 0.0f; 
    float expandSpeed = 0.0f;
    std::stack<std::pair<int, int>> cells_to_expand; 
    std::vector<std::vector<char>> corridors;
    Mesh wall; 
    Texture texture;
    GLuint instanceBuffer = 0; 

    Maze(int width, int height, float expandSpeed):
        width(width),
        height(height),
        expandSpeed(expandSpeed),
        //texture("textures/grass.bmp"),
        texture(width, height),
        wall("meshes/cube.obj") {
        corridors.resize(width, std::vector<char>(height, '#'));
        updateTextureFromCorridors();
        }
 
    void updateTextureFromCorridors() {
        std::vector<unsigned char> textureData;

        for (int y = 0; y < width; ++y) {
            for (int x = 0; x < height; ++x) {
                // Use a checkerboard pattern for the texture
                if ((x + y) % 2 == 0) {
                    textureData.push_back(255); // Red component for white
                    textureData.push_back(255); // Green component for white
                    textureData.push_back(255); // Blue component for white
                } else {
                    textureData.push_back(0);   // Red component for black
                    textureData.push_back(0);   // Green component for black
                    textureData.push_back(0);   // Blue component for black
                }
            }
        }

        texture.updateTexture(textureData);
    }

    void expand() {
        
        if (cells_to_expand.empty()) { // Maze is complete or not started
            if (corridors[1][1] == ' ') { return; } //Maze is complete
                cells_to_expand.push({1, 1}); // Start
            }

        struct Direction { int dx, dy; };
        std::vector<Direction> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; // North South West East
        std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device()()));

        auto cell = cells_to_expand.top();

        for (const auto& dir : directions) {
            int nx = cell.first + dir.dx * 2;
            int ny = cell.second + dir.dy * 2;
            if (nx >= 0 && ny >= 0 && nx < width && ny < height && corridors[nx][ny] == '#') {
                corridors[cell.first + dir.dx][cell.second + dir.dy] = ' ';
                corridors[nx][ny] = ' ';
                cells_to_expand.push({nx, ny});
                return;
                
                }
            }
        corridors[cell.first][cell.second] = ' ';
        cells_to_expand.pop();

        }

    void tick(float deltaTime) {
        expandTimer += deltaTime;
        if (expandTimer > expandSpeed) {
            expand();
            expandTimer = 0.0f;
            }
        }      

    void draw() {

        if (texture.width != width || texture.height != height) {
            texture.resize(width, height);
            updateTextureFromCorridors(); // Update texture data after resizing
            }

        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        texture.bind(1);

        glUniform1i(glGetUniformLocation(currentProgram, "mazeWidth"), width);
        glUniform1i(glGetUniformLocation(currentProgram, "mazeHeight"), height);
        glUniform1i(glGetUniformLocation(currentProgram, "corridorsTexture"), 1);

        wall.draw(width * height);
        
        }

    };