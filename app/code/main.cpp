
#include <iostream>
#include <sstream>
#include <libs/glm/glm.hpp>
#include <libs/glm/gtc/type_ptr.hpp>
#include "graphics/window.h"
#include "camera.h"
#include "sky.h"
#include "maze.h"
#include "audio/sound.h"
#include "finalizer.h"
#include "editor.h"

int main() {

    Sky sky(glm::vec3(0.1f, 1.0f, 0.1f), glm::vec3(0.529f, 0.676f, 0.701f), glm::vec3(1.0f), glm::vec3(0.016f, 0.067f, 0.074f), glm::vec3(1.000f, 0.847f, 0.775f));
    Audio audio;
    Camera camera(80.0f, 1920.0f / 1080.0f, 0.1f, 10000.0f); 
    camera.set_position(0, 30, -100);
    camera.set_target(0, 0, 0);
    Window window(1920, 1080, "Maze", true);
    Shader regularShader("shaders/regular.glsl");
    Shader postShader("shaders/post.glsl");
    Shader mazeShader("shaders/maze.glsl");
    Shader shader2d("shaders/2d.glsl");
    Texture grass("textures/grass.jpg");  
    Mesh wall("meshes/cube.obj");
    Mesh feild("meshes/feild.obj");

    Maze maze(128, 128, 3, 1.0, 5000.0, camera, false, &shader2d, &mazeShader);
    Framebuffer framebuffer(1920, 1080);
    Finalizer finalizer;  
    Editor editor(window, camera, sky, finalizer, maze); 

    while (window.is_open()) {

        #pragma region Input

            window.poll_events();

            maze.tick(window.delta_time);

            if (camera.collide) {
                
                glm::vec3 old_target = camera.target;
                glm::vec3 old_position = camera.position;

                camera.walk( // Foward And Rightward
                    (window.input(GLFW_KEY_W) - window.input(GLFW_KEY_S)) * window.delta_time * (1 +(window.input(GLFW_KEY_LEFT_SHIFT) * 2)), 
                    (window.input(GLFW_KEY_D) - window.input(GLFW_KEY_A)) * window.delta_time * (1 +(window.input(GLFW_KEY_LEFT_SHIFT) * 2)) 
                    );

                // if (camera.falling == 0) { // Jump
                //     if (window.input(GLFW_KEY_SPACE)) {
                //         camera.y_velocity = .1; // Jump Height
                //         camera.falling = 1;
                //         }
                //     }

                camera.fall(window.delta_time); // Fall (Gravity)

                if (camera.position.y < camera.headheight) { // Keep the camera above the ground
                    camera.position.y = camera.headheight;
                    camera.falling = 0;
                    camera.y_velocity = 0;
                    } 
                
                if (maze.colliding(glm::vec3(camera.position.x, old_position.y, old_position.z))) { // Collide Maze X
                    
                    camera.position.x = old_position.x; 
                    }

                if (maze.colliding(glm::vec3(old_position.x, camera.position.y - camera.headheight, old_position.z))) { // Collide Maze Y
                    camera.position.y = maze.height + camera.headheight;
                    camera.falling = 0;
                    //camera.y_velocity = 0;
                    }

                if (maze.colliding(glm::vec3(old_position.x, old_position.y, camera.position.z))) { // Collide Maze Z
                    camera.position.z = old_position.z; 
                    }
                
                camera.setTargetToSameRelativePosition(old_position, old_target);

               
            } else { 
                camera.fly( // Foward And Rightward
                    (window.input(GLFW_KEY_W) - window.input(GLFW_KEY_S)) * window.delta_time * (1 +(window.input(GLFW_KEY_LEFT_SHIFT) * 2)), 
                    (window.input(GLFW_KEY_D) - window.input(GLFW_KEY_A)) * window.delta_time * (1 +(window.input(GLFW_KEY_LEFT_SHIFT) * 2)) 
                    );
                }

            

            // Toggle cursor
            if (window.input_released(GLFW_KEY_ESCAPE)) {
                if (glfwGetInputMode(window.GLFW_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                    glfwSetInputMode(window.GLFW_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    glfwSetInputMode(window.GLFW_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    }    
                }
            
            // Look around
            if (glfwGetInputMode(window.GLFW_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) { // Look around only if the cursor is disabled
                camera.look(window.mouse_delta_x, window.mouse_delta_y);
                }
                
            #pragma endregion

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        framebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sky.draw();
        regularShader.bind();
        sky.bind(&regularShader); 
        camera.bind(&regularShader);
        grass.bind(0); 
        feild.draw();
        mazeShader.bind();
        sky.bind(&mazeShader);
        camera.bind(&mazeShader);
        maze.draw();
        framebuffer.unbind();
        finalizer.draw(&postShader, &framebuffer);
        editor.edit();
        window.swap_buffers();

        }
        
    return 0;
 
    }