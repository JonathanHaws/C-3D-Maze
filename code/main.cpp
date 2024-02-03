
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <sstream>
#include <camera.h>
#include <mesh.h>
#include <texture.h>
#include <shader.h>
#include <maze.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

float cameraYaw = 0.0f;
float cameraPitch = 0.0f;

#pragma region Window Callbacks

    void window_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        }
      
        #pragma endregion

int main() {
    
    #pragma region Window

        #pragma region Create Window 
            if (!glfwInit()) {
                std::cerr << "Failed to initialize GLFW\n";
                return -1;
                }

            GLFWmonitor* monitor = true ? glfwGetPrimaryMonitor() : nullptr; // fullscreen
            GLFWwindow* window = glfwCreateWindow(1920, 1080, "Maze Game", monitor, nullptr);
            if (!window) {
                std::cerr << "Failed to create GLFW window\n";
                glfwTerminate();
                return -1;
                }
            #pragma endregion

        #pragma region Create Context
            glfwMakeContextCurrent(window);

            GLenum err = glewInit();
            if (err != GLEW_OK) {
                std::cerr << "Failed to initialize GLEW\n";
                glfwTerminate();
                return -1;
                }
            #pragma endregion

            glfwSetWindowSizeCallback(window, window_size_callback);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            bool escapeKeyPressed = false;
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            double prevMouseX = 0.0;
            double prevMouseY = 0.0;
            float xoffset = mouseX - prevMouseX;
            float yoffset = mouseY - prevMouseY;

            double lastFrameTime = glfwGetTime();
             
        #pragma endregion

    Camera camera;
    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Texture grass("textures/grass.bmp");
    Texture stone("textures/stone.bmp");
    Mesh wall("meshes/cube.obj");
    Mesh feild("meshes/feild.obj");
    Maze maze(31, 31, 0.0);

    #pragma region Gui 

        #pragma region setup

            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 130");
            ImGui::StyleColorsDark();
            float sunPosX = 0.1f;
            float sunPosY = 1.0f;
            float sunPosZ = 0.1f;
            int mazeWidth = 31;
            int mazeHeight = 31;
            float mazeExpandTimer = -3.0f;
            float expansionSpeed = 0.02f; 
            float prevExpansionSpeed = 0.02f;
            bool paused = false;
            
            #pragma endregion

        #pragma region Styling
            
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
            
            #pragma endregion

    #pragma endregion

    while (!glfwWindowShouldClose(window)) {

        #pragma region Input

            glfwPollEvents();
            double currentFrameTime = glfwGetTime();
            float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
            lastFrameTime = currentFrameTime;
         
            #pragma region camera movement

                float forward = (glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S)) * 12 * deltaTime;
                float right = (glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)) * 12 * deltaTime;
                glm::vec3 forwardVector = glm::normalize(camera.target - camera.position);
                glm::vec3 rightVector = glm::normalize(glm::cross(forwardVector, glm::vec3(0, 1, 0))); 
                camera.position += forward * forwardVector + right * rightVector;
                camera.target += forward * forwardVector + right * rightVector;
                

                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
                    // Only toggle cursor mode if Escape key was previously pressed
                    if (escapeKeyPressed) {
                        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        } else {
                            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                            }
                        escapeKeyPressed = false; // Reset the state of the Escape key
                        }
                } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    escapeKeyPressed = true; // Set the state of the Escape key to pressed
                }

                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                xoffset = mouseX - prevMouseX;
                yoffset = mouseY - prevMouseY;
                prevMouseX = mouseX;
                prevMouseY = mouseY;

                if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {

                    float sensitivity = 0.1f;
                    cameraYaw += xoffset * sensitivity;
                    cameraPitch += -yoffset * sensitivity;
                    
                    xoffset = 0;
                    yoffset = 0;

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
                
                #pragma region Clear
                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();
                    #pragma endregion

                #pragma region Controls
                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // Set window position to top-left corner
                ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1)); // Set width to auto-resize

                ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

                if (ImGui::BeginMenuBar()) {
                    if (ImGui::BeginMenu("Options")) {
                        if (ImGui::MenuItem("Collapse")) {
                            ImGui::SetWindowCollapsed(!ImGui::IsWindowCollapsed());
                            }
                        ImGui::EndMenu();
                        }
                    ImGui::EndMenuBar();
                    }

                
                #pragma region Maze Controls

                    ImGui::SliderInt("Width", &mazeWidth, 1, 100);
                    ImGui::SliderInt("Height", &mazeHeight, 1, 100);
                    
                    #pragma region Expansion Speed
                        ImGui::SliderFloat("Speed", &expansionSpeed, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                        // Check if the expansion speed has changed
                        if (prevExpansionSpeed != expansionSpeed) {
                            maze.setExpandSpeed(expansionSpeed);
                            prevExpansionSpeed = expansionSpeed; 
                            }
                        #pragma endregion

                    if (ImGui::Button("Reset")) {
                        maze = Maze(mazeWidth, mazeHeight, expansionSpeed); 
                        paused = true;
                        }

                    ImGui::SameLine();
                    if (!paused) {
                        if (ImGui::Button("Stop Expanding")) {
                            paused = true;
                            }
                        maze.tick(deltaTime);
                    } else {
                        if (ImGui::Button("Expand")) {
                            paused = false;
                            }
                        }

                    #pragma region Expand Once
                        
                        ImGui::SameLine();
                        if (ImGui::Button("Expand Once")) {
                            maze.expand();
                            mazeExpandTimer = 0.0f; // Reset the timer for maze expansion
                            }
                        
                        #pragma endregion


                #pragma region Sun Position
                    
                    ImGui::SliderFloat("Sun X", &sunPosX, -10.0f, 10.0f); 
                    ImGui::SliderFloat("Sun Y", &sunPosY, 1.0f, 10.0f);
                    ImGui::SliderFloat("Sun Z", &sunPosZ, -10.0f, 10.0f);

                    glUniform3fv(glGetUniformLocation(shader.getID(), "lightDirection"), 1, glm::value_ptr(glm::vec3(sunPosX, sunPosY, sunPosZ)));
                    
                    #pragma endregion

                #pragma region Fullscreen
                    
                    if (ImGui::Button("Fullscreen")) {
                        
                        GLFWmonitor* currentMonitor = glfwGetWindowMonitor(window);
                        if (currentMonitor != nullptr) {
                            // If the window is in fullscreen mode, switch to windowed mode
                            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                            glfwSetWindowMonitor(window, nullptr, 0, 50, 1920, 1080, GLFW_DONT_CARE);
                        } else {
                            // If the window is in windowed mode, switch to fullscreen mode
                            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                            }
                        
                        }
                    
                    #pragma endregion

                ImGui::End();

                #pragma region Draw
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                    #pragma endregion

                #pragma endregion

            glfwSwapBuffers(window);

            
            
            #pragma endregion
    
                #pragma endregion
    
    }

    #pragma region Cleanup
        
        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
        
        #pragma endregion

    }