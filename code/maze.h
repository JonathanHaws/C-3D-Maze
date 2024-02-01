#pragma once

#include <vector>
#include <iostream>
#include <stack>
#include <algorithm> // shuffle random
#include <random>

class Maze {
    
    public:
        
        Maze(int width, int height, float expandSpeed) : width(width), height(height), expandTimer(0.0f), expandSpeed(expandSpeed) {
            corridors.resize(width, std::vector<char>(height, '#'));
            }

        const std::vector<std::vector<char>>& getCorridors() const {
                return corridors;
            }

        int getWidth() const {
            return width;
            }
        
        float getExpandSpeed() const {
            return expandSpeed;
            }

        void setExpandSpeed(float speed) {
            expandSpeed = speed;
            }

        int getHeight() const {
            return height;
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
                if (nx >= 0 && ny >= 0 && nx < getWidth() && ny < getHeight() && corridors[nx][ny] == '#') {
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
        
    private:
        
        int width;
        int height;
        float expandTimer;
        float expandSpeed;
        std::stack<std::pair<int, int>> cells_to_expand;
        std::vector<std::vector<char>> corridors;
    
    };