
#include <iostream>
#include <sstream>
#include <window.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <camera.h>
#include <mesh.h>
#include <texture.h>
#include <shader.h>
#include <maze.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main() {
    
    Window window(1920, 1080, "Maze", true);
    Camera camera;
    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Texture grass("textures/grass.bmp");
    Texture stone("textures/stone.bmp");
    Mesh wall("meshes/cube.obj");
    Mesh feild("meshes/feild.obj");
    Maze maze(31, 31, 0.0);

    #pragma region Gui 

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window.GLFW_window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
        ImGui::StyleColorsDark();

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
        glm::vec3 ambientColor = glm::vec3(0.248f, 0.352f, 0.402f);
        glm::vec3 lightColor = glm::vec3(0.848f, 0.692f, 0.570f);
        
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
            
        #pragma endregion

    #pragma endregion

    while (window.is_open()) {

        #pragma region Input

            window.poll_events();
         
            #pragma region camera movement
            

                float camera_speed = 1 + (window.input(GLFW_KEY_LEFT_SHIFT) * 2); // If left shift is pressed, double the speed
                float forward = (window.input(GLFW_KEY_W) - window.input(GLFW_KEY_S)) * 12 * camera_speed * window.delta_time;
                float right = (window.input(GLFW_KEY_D) - window.input(GLFW_KEY_A)) * 12 * camera_speed * window.delta_time;
                glm::vec3 forwardVector = glm::normalize(camera.target - camera.position);
                glm::vec3 rightVector = glm::normalize(glm::cross(forwardVector, glm::vec3(0, 1, 0))); 
                camera.position += forward * forwardVector + right * rightVector;
                camera.target += forward * forwardVector + right * rightVector;

                if (window.input_released(GLFW_KEY_ESCAPE)) {
                    if (glfwGetInputMode(window.GLFW_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                        glfwSetInputMode(window.GLFW_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    } else {
                        glfwSetInputMode(window.GLFW_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        }    
                    }
                

                if (glfwGetInputMode(window.GLFW_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {

                    float cameraYaw = camera.get_yaw();
                    float cameraPitch = camera.get_pitch();

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

            #pragma endregion

        #pragma region Draw

            #pragma region Clear

                //glEnable(GL_CULL_FACE);
                //glCullFace(GL_BACK);
                
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                #pragma endregion

            #pragma region Draw maze
                
                auto corridors = maze.getCorridors();
                for (int i = 0; i < corridors.size(); ++i) {
                    for (int j = 0; j < corridors[i].size(); ++j) {
                        if (corridors[i][j] == '#') {
                            float wallX = (j - maze.getWidth() / 2) * 2.0f;
                            float wallZ = (maze.getHeight() / 2 - i) * 2.0f;
                            camera.draw(wall.getVAO(), wall.getEBO(), wall.getVertexCount(), wallX, 0, wallZ, stone.getID(), shader.getID());
                            }
                        }
                    }

                #pragma endregion

            camera.draw(feild.getVAO(), feild.getEBO(), feild.getVertexCount(), 0, 0, 0, grass.getID(), shader.getID());

            #pragma region Gui 
                
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
                            ImGui::SliderInt("Width", &mazeWidth, 1, 400);
                            ImGui::SliderInt("Height", &mazeHeight, 1, 400);
                            ImGui::SliderFloat("Speed", &expansionSpeed, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                            if (ImGui::Button("Reset")) {
                                maze = Maze(mazeWidth, mazeHeight, expansionSpeed); 
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

                    ImGui::End();

                glUniform3fv(glGetUniformLocation(shader.getID(), "objectColor"), 1, glm::value_ptr(objectColor));
                glUniform3fv(glGetUniformLocation(shader.getID(), "ambientColor"), 1, glm::value_ptr(ambientColor));
                glUniform3fv(glGetUniformLocation(shader.getID(), "lightColor"), 1, glm::value_ptr(lightColor));
                glUniform3fv(glGetUniformLocation(shader.getID(), "lightDirection"), 1, glm::value_ptr(glm::vec3(sunPosX, sunPosY, sunPosZ)));
                
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                #pragma endregion

            window.swap_buffers();

            #pragma endregion

        }
        
        return 0;
        
    }