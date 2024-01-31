#pragma once

#include <vector>
#include <iostream>
#include <stack>
#include <algorithm> // shuffle random
#include <random>

class Maze {
    
    public:
        
        Maze(int width, int height) : width(width), height(height) {
            corridors.resize(height, std::vector<char>(width, '#'));
            }

        const std::vector<std::vector<char>>& getCorridors() const {
                return corridors;
            }

        int getWidth() const {
            return width;
            }

        int getHeight() const {
            return height;
            }
        
        void expand() {
            
            if (cells_to_expand.empty()) { // Maze is complete or not started
                if (corridors[getWidth() -2][getHeight() -2] == ' ') { return; } //Maze is complete
                    cells_to_expand.push({getWidth() -2, getHeight() -2}); // Start
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

            
        
    private:
        
        int width;
        int height;
        std::stack<std::pair<int, int>> cells_to_expand;
        std::vector<std::vector<char>> corridors;
    
    };