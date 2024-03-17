
#include <iostream>
#include <sstream>
#include "libs/glm/glm.hpp"
#include "graphics/graphics.h"
#include "sky.h"
#include "maze.h"
#include "audio/sound.h"
#include "editor.h"
//#include "post.h"

int main() {

    bool fog = true;
    float fog_distance = 0.9f;
    float fog_falloff = 0.15f;
    glm::vec3 fog_color = glm::vec3(0.529f, 0.676f, 0.701f);
    bool blur = false;
    int blurRadius = 3;
    bool ambientOcclusion = false;
    bool occlusionBuffer = false;
    int occlusionRadius = 2;
    float occlusionThreshold = 0.5f;
    float occlusionStrength = 1.0f;
    bool depthBuffer = false;
    float exposure = 1.0f;
    float gamma = 1.0f;

    //Post post;  
    Sky sky(glm::vec3(0.1f, 1.0f, 0.1f), glm::vec3(0.529f, 0.676f, 0.701f), glm::vec3(1.0f), glm::vec3(0.016f, 0.067f, 0.074f), glm::vec3(1.000f, 0.847f, 0.775f));
    Audio audio;
    Camera camera(80.0f, 1920.0f / 1080.0f, 0.1f, 10000.0f); 
    camera.set_position(0, 30, -100);
    camera.set_target(0, 0, 0);
    Window window(1920, 1080, "Maze", true);
    Framebuffer framebuffer(1920, 1080);
    Shader regularShader("shaders/regular.glsl");
    Shader postShader("shaders/post.glsl");
    Shader mazeShader("shaders/maze.glsl");
    Texture grass("textures/grass.jpg");  
    Mesh wall("meshes/cube.obj");
    Mesh feild("meshes/feild.obj");
    Mesh quad("meshes/quad.obj");
    Mesh sword("meshes/sword.obj");
    Maze maze(128, 128, 3, 0.5, 5000.0, camera, false);
    Editor editor(window, camera, sky, maze, depthBuffer, exposure, gamma, fog, fog_distance, fog_falloff, fog_color, blur, blurRadius, ambientOcclusion, occlusionBuffer, occlusionRadius, occlusionThreshold, occlusionStrength);

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
        sky.draw();

        framebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sky.draw();

        regularShader.bind();
        sky.bind(&regularShader); 
        regularShader.setMat4("Model",  glm::mat4(1.0f));
        regularShader.setMat4("View", camera.viewMatrix());
        regularShader.setMat4("Projection", camera.projectionMatrix());   
        grass.bind(0); 
        feild.draw();

        mazeShader.bind();
        sky.bind(&mazeShader);
        mazeShader.setMat4("Model",  glm::mat4(1.0f));
        mazeShader.setMat4("View", camera.viewMatrix());
        mazeShader.setMat4("Projection", camera.projectionMatrix());
        maze.draw();

        framebuffer.unbind();

        postShader.bind();
        postShader.setFloat("exposure", exposure);
        postShader.setFloat("gamma", gamma);
        postShader.setInt("fog", fog);
        postShader.setFloat("fog_distance", fog_distance);
        postShader.setFloat("fog_falloff", fog_falloff);
        postShader.setVec3("fog_color", fog_color.x, fog_color.y, fog_color.z);
        postShader.setInt("blur", blur);
        postShader.setInt("blurRadius", blurRadius);
        postShader.setInt("depthBuffer", depthBuffer);
        postShader.setInt("ambientOcclusion", ambientOcclusion);
        postShader.setInt("occlusionBuffer", occlusionBuffer);
        postShader.setInt("occlusionRadius", occlusionRadius);
        postShader.setFloat("occlusionThreshold", occlusionThreshold);
        postShader.setFloat("occlusionStrength", occlusionStrength);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.color_texture);
        glUniform1i(glGetUniformLocation(postShader.id, "colorTexture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffer.depth_texture);
        glUniform1i(glGetUniformLocation(postShader.id, "depthTexture"), 1);

        quad.draw();
        editor.edit();
        window.swap_buffers();

        }
        
    return 0;
 
    }