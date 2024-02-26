
#include <iostream>
#include <sstream>
#include <window.h>
#include <graphics.h>
#include <glm/glm.hpp>
#include <maze.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <audio.h>

int main() {

    bool god_mode = true;
    
    Audio audio;
    Window window(1920, 1080, "Maze", true);
    Camera camera; 
    Framebuffer framebuffer(1920, 1080);
    Shader regularShader("shaders/regular_v.glsl", "shaders/regular_f.glsl");
    Shader postShader("shaders/post_v.glsl", "shaders/post_f.glsl");
    Shader mazeShader("shaders/maze_v.glsl", "shaders/maze_f.glsl");
    Texture grass("textures/grass.bmp");
    Texture stone("textures/stone.bmp");     
    Mesh wall("meshes/cube.obj");
    Mesh feild("meshes/feild.obj");
    Mesh quad("meshes/quad.obj");
    Mesh sword("meshes/sword.obj");
    Maze maze(64, 64, 0.0);
    regularShader.bind();

    #pragma region Gui 

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window.GLFW_window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL; // Disable saving users gui settings / state to ini file

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);

        int mazeWidth = 31;
        int mazeHeight = 31;
        float mazeExpandTimer = -3.0f;
        float expansionSpeed = 0.02f; 
        float prevExpansionSpeed = 0.02f;
        bool paused = false;

        float sunPosX = 0.1f;
        float sunPosY = 1.0f;
        float sunPosZ = 0.1f;    
        glm::vec3 objectColor = glm::vec3(1.000f, 1.000f, 1.000f);
        glm::vec3 ambientColor = glm::vec3(0.251f, 0.316f, 0.324f);
        glm::vec3 lightColor = glm::vec3(0.853f, 0.609f, 0.418f);
 
        bool fog = true;
        float fog_distance = 0.2f;
        float fog_falloff = 0.8f;
        glm::vec3 fog_color = glm::vec3(0.059f, 0.059f, 0.059f);

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
      
        #pragma endregion

    #pragma endregion

    while (window.is_open()) {

        //std::cout << "FPS: " << 1.0 / window.delta_time << std::endl;

        #pragma region Input

            window.poll_events();
        
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

        regularShader.bind();
        regularShader.setVec3("lightDirection", sunPosX, sunPosY, sunPosZ);
        regularShader.setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);
        regularShader.setVec3("ambientColor", ambientColor.x, ambientColor.y, ambientColor.z);
        regularShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        framebuffer.bind();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        grass.bind(0);
        mazeShader.setMat4("Model",  glm::mat4(1.0f));
        mazeShader.setMat4("View", camera.viewMatrix());
        mazeShader.setMat4("Projection", camera.projectionMatrix());
        
        feild.draw();

        GLuint modelUniformLocation = glGetUniformLocation(mazeShader.id, "Model");
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 50.0f, 0.0f));
        glm::mat4 transformMatrix = translationMatrix * scaleMatrix;
        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));

        maze.texture.bind(0);
        glUniform1i(glGetUniformLocation(postShader.id, "texture_diffuse1"), 0);
        quad.draw();

        stone.bind(0);
        mazeShader.bind();
        mazeShader.setMat4("Model",  glm::mat4(1.0f));
        mazeShader.setMat4("View", camera.viewMatrix());
        mazeShader.setMat4("Projection", camera.projectionMatrix());

        mazeShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        mazeShader.setVec3("lightDirection", sunPosX, sunPosY, sunPosZ);
        mazeShader.setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);
        mazeShader.setVec3("ambientColor", ambientColor.x, ambientColor.y, ambientColor.z);

        maze.draw();
        mazeShader.unbind();

        framebuffer.unbind();

        postShader.bind();
        postShader.setInt("fog", fog);
        postShader.setFloat("fog_distance", fog_distance);
        postShader.setFloat("fog_falloff", fog_falloff);
        postShader.setVec3("fog_color", fog_color.x, fog_color.y, fog_color.z);
        postShader.setFloat("exposure", exposure);
        postShader.setFloat("gamma", gamma);
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

        if (god_mode) {

        #pragma region Debug 
            
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // Set window position to top-left corner
            ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1)); // Set width to auto-resize
            ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
                
                if (ImGui::BeginMenuBar()) {
                    ImGui::SetWindowCollapsed(!ImGui::IsWindowCollapsed());
                    }        

                    if (ImGui::CollapsingHeader("Window")) {
                            
                            if (ImGui::MenuItem("Fullscreen")) {
                                window.set_fullscreen(!window.is_fullscreen());
                                }
                            if (ImGui::MenuItem("Exit")) {
                                window.close();
                                }
                            } 

                    if (ImGui::CollapsingHeader("Camera")) {
                        ImGui::SliderFloat("FOV", &camera.fov, 1.0f, 179.0f);
                        ImGui::SliderFloat("Near", &camera.nearPlane, 0.1f, 100.0f);
                        ImGui::SliderFloat("Far", &camera.farPlane, 0.1f, 10000.0f);
                        }

                    if (ImGui::CollapsingHeader("Maze")) {
                        ImGui::SliderInt("Width", &maze.width, 1, 2048);
                        ImGui::SliderInt("Height", &maze.height, 1, 2048);
                        ImGui::SliderFloat("Speed", &expansionSpeed, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                        if (ImGui::Button("Reset")) {
                            //audio.playSound("audio/expand.wav");
                            //audio.generateSawtoothWave
                            maze.reset();
                            paused = true;
                            }
                        ImGui::SameLine();
                        if (!paused) {
                            if (ImGui::Button("Stop Expanding")) {
                                paused = true;
                                }
                            maze.tick(window.delta_time);
                        } else {
                            if (ImGui::Button("Expand")) {
                                paused = false;
                                }
                            }
                        ImGui::SameLine();
                        if (ImGui::Button("Expand Once")) {
                            //audio.playSound("audio/expand.wav");
                            maze.expand();
                            mazeExpandTimer = 0.0f; // Reset the timer for maze expansion
                            }
                        }

                    if (ImGui::CollapsingHeader("Lighting")) {
                        ImGui::SliderFloat("Sun X", &sunPosX, -10.0f, 10.0f); 
                        ImGui::SliderFloat("Sun Y", &sunPosY, 1.0f, 10.0f);
                        ImGui::SliderFloat("Sun Z", &sunPosZ, -10.0f, 10.0f);

                        ImGui::ColorEdit3("Object Color", glm::value_ptr(objectColor));
                        ImGui::ColorEdit3("Ambient Color", glm::value_ptr(ambientColor));
                        ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
                        }

                    if (ImGui::CollapsingHeader("Post Shader")) {
                        ImGui::Checkbox("Depth Buffer", &depthBuffer);
                        ImGui::SliderFloat("Exposure", &exposure, 0.0f, 10.0f);
                        ImGui::SliderFloat("Gamma", &gamma, 0.0f, 10.0f);

                        ImGui::Checkbox("Fog", &fog);
                        if (fog) { 
                            ImGui::SliderFloat("Distance", &fog_distance, 0.0f, 1.0f); 
                            ImGui::SliderFloat("Falloff", &fog_falloff, 0.0f, 1.0f);
                            ImGui::ColorEdit3("Color", glm::value_ptr(fog_color));
                            }
                        
                        ImGui::Checkbox("Blur", &blur);
                        if (blur) { 
                            ImGui::SliderInt("Blur Radius", &blurRadius, 1, 10); 
                            }

                        ImGui::Checkbox("Ambient Occlusion", &ambientOcclusion);
                        if (ambientOcclusion) { 
                            ImGui::Checkbox("Occlusion Buffer", &occlusionBuffer);
                            ImGui::SliderInt("Radius", &occlusionRadius, 1, 10); 
                            ImGui::SliderFloat("Threshold", &occlusionThreshold, 0.0f, 1.0f);
                            ImGui::SliderFloat("Strength", &occlusionStrength, 0.0f, 100.0f);
                            }
                        }

                ImGui::End();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                #pragma endregion

        }

        window.swap_buffers();

        #pragma endregion

        }
        
        return 0;
   
    }