#include <iostream>
#include <random>
#include <algorithm> // shuffle random
#include <conio.h> // input getch()
#include <cstdlib> // clear console command (system("cls"))
#include <vector> // vector for dynamic array
#include <windows.h> // sleep command
using namespace std;

void print_maze(const vector<vector<char>>& maze) {
    system("cls"); // clear console
    string output;
    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze[i].size(); j++) {
            for (int l = 0; l < 2; l++) { // Print each character twice
                output += maze[i][j]; // fix squished look by printing horizontally twice
            }
        }
        output += "\n";
    }
    cout << output;
}

// How the recursive backtracker algorithm works:
// Check if there is a wall 2 spaces in random direction. 
// If so, remove two walls to get there and recursively expand. 
// If not stop recursion and check other directions.
// If all directions are walls, backtrack to previous position and check other directions.
// One call to exapnd will create whole maze! 

void expand(int x, int y, vector<vector<char>>& maze) {
    print_maze(maze); 
    Sleep(1); // Slow down maze generation to be able and visually see it  
    maze[x][y] = ' '; // Mark as visited
    vector<pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    shuffle(directions.begin(), directions.end(), mt19937(random_device()()));
    for (auto [dx, dy] : directions) {
        int nx = x + dx * 2, ny = y + dy * 2;
        if (nx >= 0 && ny >= 0 && nx < maze.size() && ny < maze[0].size() && maze[nx][ny] == '#') {
            maze[x + dx][y + dy] = ' ';
            expand(nx, ny, maze);
        }
    }
}
         
vector<vector<char>> generate_maze(int rows, int cols) {
    vector<vector<char>> maze(rows, vector<char>(cols, '#')); // Fill with walls  
    expand(1, 1, maze);
    maze[1][1] = 'P'; // start
    maze[rows-2][cols-2] = 'F'; // end
    print_maze(maze);
    return maze;
}

bool move(int xv, int yv, int& x, int& y, vector<vector<char>>& maze) {
    int new_x = x + xv;
    int new_y = y + yv;
    if (new_x < 0 || new_x >= maze.size() || new_y < 0 || new_y >= maze[0].size()) {
        return false; // new position is out of bounds, move failed
    }
    if (maze[new_x][new_y] == 'F') {
        return true; // reached destination
    }
    if (maze[new_x][new_y] == ' ') { // move if not colliding with a wall
        maze[new_x][new_y] = 'P'; // mark new position
        maze[x][y] = ' '; // clear previous position
        x = new_x;
        y = new_y;
    }
    return false;
}

int main() {

    cout << "Hello!" << endl;
    Sleep(1000);
    cout << "Whats your name?" << endl;
    string name;
    cin >> name;
    cout << "Hello " << name << "!" << endl;
    Sleep(1000);
    cout << "My name is Jonathan. And this is my first C++ program!" << endl;
    Sleep(2000);
    cout << "A FUN MAZE GAME!!!" << endl;
    Sleep(2000);
    cout << "But beaware! This is no place for those easily lost" << endl;
    Sleep(2000);
    cout << "Many go missing within these walls" << endl;
    Sleep(2000);
    cout << "Do you think you can ESCAPE!?" << endl;
    Sleep(2000);

    while (true) {
        
        cout << "How big of a maze do you think you can handle now?" << endl;
        Sleep(1000);
        int width, height;
        cout << "Width?" << endl;
        cin >> width;
        cout << "Height?" << endl;
        cin >> height;
        Sleep(1000);
        cout << "Good luck! Use WASD tO control the player" << endl;
        Sleep(1000);
        cout << "Starting Maze Generation..." << endl;
        Sleep(1000);

        if ( width < 5) { width = 5;} // Make size is at least 5
        if ( height < 5) { height = 5;}
        if ( width % 2 == 0) { width += 1;} // Make size is odd
        if ( height % 2 == 0) { height += 1;} 
        
        vector<vector<char>> maze = generate_maze(height, width);
        int x = 1, y = 1;
        bool complete = false;
        while (!complete) {
            switch (_getch()) {
                case 'w': case 'W': complete = move(-1, 0, x, y, maze); break;
                case 'a': case 'A': complete = move(0, -1, x, y, maze); break;
                case 's': case 'S': complete = move(1, 0, x, y, maze); break;
                case 'd': case 'D': complete = move(0, 1, x, y, maze); break;
            }
            print_maze(maze);
        }
        cout << "Wow you managed to traverse the maze!" << endl;
        Sleep(5000);
    }
    return 0;
}
