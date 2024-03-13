
#include <iostream>
#include <sstream>
#include "libs/glm/glm.hpp"
#include "graphics/graphics.h"
#include "maze.h"
#include "editor.h"
#include "audio/sound.h"

int main() {

    float sunX = 0.1f;
    float sunY = 1.0f;
    float sunZ = 0.1f;   
    glm::vec3 skyColor = glm::vec3(0.529f, 0.676f, 0.701f); 
    glm::vec3 objectColor = glm::vec3(1.000f, 1.000f, 1.000f);
    glm::vec3 ambientColor = glm::vec3(0.251f, 0.316f, 0.324f);
    glm::vec3 lightColor = glm::vec3(0.853f, 0.609f, 0.418f);
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

    Audio audio;
    Camera camera(80.0f, 1920.0f / 1080.0f, 0.1f, 10000.0f); 
    camera.set_position(0, 200, -200);
    camera.set_target(0, 0, 0);
    Window window(1920, 1080, "Maze", true);
    Framebuffer framebuffer(1920, 1080);
    Shader regularShader("shaders/regular.glsl");
    Shader postShader("shaders/post.glsl");
    Shader mazeShader("shaders/maze.glsl");
    Texture grass("textures/grass.jpg");
    Texture stone("textures/stone.bmp");     
    Mesh wall("meshes/cube.obj");
    Mesh feild("meshes/feild.obj");
    Mesh quad("meshes/quad.obj");
    Mesh sword("meshes/sword.obj");
    Maze maze(512, 512, 1000.0, camera, true);
    Editor editor(window, camera, maze, sunX, sunY, sunZ, skyColor, objectColor, ambientColor, lightColor, depthBuffer, exposure, gamma, fog, fog_distance, fog_falloff, fog_color, blur, blurRadius, ambientOcclusion, occlusionBuffer, occlusionRadius, occlusionThreshold, occlusionStrength);

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
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //framebuffer.bind();
        // glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        regularShader.bind();
        regularShader.setVec3("lightDirection", sunX, sunY, sunZ);
        regularShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        regularShader.setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);
        regularShader.setVec3("ambientColor", ambientColor.x, ambientColor.y, ambientColor.z);
        regularShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        regularShader.setMat4("Model",  glm::mat4(1.0f));
        regularShader.setMat4("View", camera.viewMatrix());
        regularShader.setMat4("Projection", camera.projectionMatrix());   
        grass.bind(0); 
        feild.draw();

        stone.bind(0);
        mazeShader.bind();
        mazeShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        mazeShader.setVec3("lightDirection", sunX, sunY, sunZ);
        mazeShader.setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);
        mazeShader.setVec3("ambientColor", ambientColor.x, ambientColor.y, ambientColor.z);
        mazeShader.setMat4("Model",  glm::mat4(1.0f));
        mazeShader.setMat4("View", camera.viewMatrix());
        mazeShader.setMat4("Projection", camera.projectionMatrix());
        maze.draw();

        // framebuffer.unbind();

        // postShader.bind();
        // postShader.setFloat("exposure", exposure);
        // postShader.setFloat("gamma", gamma);
        // postShader.setInt("fog", fog);
        // postShader.setFloat("fog_distance", fog_distance);
        // postShader.setFloat("fog_falloff", fog_falloff);
        // postShader.setVec3("fog_color", fog_color.x, fog_color.y, fog_color.z);
        // postShader.setInt("blur", blur);
        // postShader.setInt("blurRadius", blurRadius);
        // postShader.setInt("depthBuffer", depthBuffer);
        // postShader.setInt("ambientOcclusion", ambientOcclusion);
        // postShader.setInt("occlusionBuffer", occlusionBuffer);
        // postShader.setInt("occlusionRadius", occlusionRadius);
        // postShader.setFloat("occlusionThreshold", occlusionThreshold);
        // postShader.setFloat("occlusionStrength", occlusionStrength);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, framebuffer.color_texture);
        // glUniform1i(glGetUniformLocation(postShader.id, "colorTexture"), 0);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, framebuffer.depth_texture);
        // glUniform1i(glGetUniformLocation(postShader.id, "depthTexture"), 1);

        // quad.draw();
        editor.edit();
        window.swap_buffers();

        }
        
    
    return 0;
 
    }