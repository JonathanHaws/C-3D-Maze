
#include <iostream>
#include <sstream>
#include "libs/glm/glm.hpp"
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

    Maze maze(128, 128, 3, 0.5, 5000.0, camera, false, &shader2d, &mazeShader);
    Framebuffer framebuffer(1920, 1080);
    Finalizer finalizer;  
    Editor editor(window, camera, sky, finalizer, maze); 

    while (window.is_open()) {

        #pragma region Input

            window.poll_events();

            maze.tick(window.delta_time);
        
            // Camera movement
            float camera_speed = 1 + (window.input(GLFW_KEY_LEFT_SHIFT) * 2); // If left shift is pressed, double the speed
            float forward = (window.input(GLFW_KEY_W) - window.input(GLFW_KEY_S)) * 12 * camera_speed * window.delta_time;
            float right = (window.input(GLFW_KEY_D) - window.input(GLFW_KEY_A)) * 12 * camera_speed * window.delta_time;
            glm::vec3 forwardVector = glm::normalize(camera.target - camera.position);
            glm::vec3 rightVector = glm::normalize(glm::cross(forwardVector, glm::vec3(0, 1, 0))); 
            camera.position += forward * forwardVector + right * rightVector;
            camera.target += forward * forwardVector + right * rightVector;

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

                float cameraYaw = camera.yaw();
                float cameraPitch = camera.pitch();

                float sensitivity = 0.1f;
                cameraYaw += window.mouse_delta_x * sensitivity;
                cameraPitch += -window.mouse_delta_y * sensitivity;
                
                if (cameraPitch > 89.0f) cameraPitch = 89.0f; // Clamp the pitch to prevent the camera from flipping
                if (cameraPitch < -89.0f) cameraPitch = -89.0f;
                glm::vec3 front;
                front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
                front.y = sin(glm::radians(cameraPitch));
                front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
                camera.target = camera.position + glm::normalize(front);

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