#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <sstream>
#include <camera.h>
#include <mesh.h>
#include <texture.h>
#include <shader.h>
#include <window.h>
#include <maze.h>
#include <gui.h>

int main() {
    
    Camera camera;
    Window window(1920, 1080, "Maze Game", true);
    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Texture grass("textures/grass.bmp"); //std::cout << grass.getID() << std::endl;
    Texture stone("textures/stone.bmp"); //std::cout << stone.getID() << std::endl; 
    Mesh wall("meshes/cube.obj"); //std::cout << wall.getVAO() << std::endl;
    Mesh feild("meshes/feild.obj"); //std::cout << feild.getVAO() << std::endl;
    Maze maze(31, 31, 0.0);
    Gui gui(window, maze, shader.getID());
  
    while (window.isOpen()) {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto corridors = maze.getCorridors();
        for (int i = 0; i < corridors.size(); ++i) {
            for (int j = 0; j < corridors[i].size(); ++j) {
                if (corridors[i][j] == '#') {
                    // Calculate the position of the wall based on maze coordinates
                    float wallX = (j - maze.getWidth() / 2) * 2.0f;
                    float wallZ = (maze.getHeight() / 2 - i) * 2.0f;
                    camera.draw(wall.getVAO(), wall.getEBO(), wall.getVertexCount(), wallX, 0, wallZ, stone.getID(), shader.getID());
                    }
                }
            }

        camera.draw(feild.getVAO(), feild.getEBO(), feild.getVertexCount(), 0,0,0, grass.getID(), shader.getID());

        gui.draw();

        float forward = window.input(GLFW_KEY_W) ? 1.0f : (window.input(GLFW_KEY_S) ? -1.0f : 0.0f);
        float strafe = window.input(GLFW_KEY_D) ? 1.0f : (window.input(GLFW_KEY_A) ? -1.0f : 0.0f);
        float turn = window.input(GLFW_KEY_RIGHT) ? 1.0f : (window.input(GLFW_KEY_LEFT) ? -1.0f : 0.0f);

        camera.move(forward, strafe, turn, window.getDeltaTime());
        camera.setAspectRatio(window.getAspectRatio());
        
        window.tick();
    }
    return 0;
}