    
#pragma once
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Window.h"
#include "Maze.h"

class Gui {
    private:
        float sunPosX = 0.1f;
        float sunPosY = 1.0f;
        float sunPosZ = 0.1f;
        int mazeWidth = 31;
        int mazeHeight = 31;
        float mazeExpandTimer = -3.0f;
        float expansionSpeed = 0.02f; 
        float prevExpansionSpeed = 0.02f;
        bool paused = false;
        Window& window;
        Maze& maze;
        unsigned int shaderProgram;
        
    public:

        Gui(Window& window, Maze& maze, unsigned int shader) : maze(maze), window(window), shaderProgram(shader){
            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
            ImGui_ImplOpenGL3_Init("#version 130");
            ImGui::StyleColorsDark();
            expansionSpeed = maze.getExpandSpeed();
            
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
            } 
        
        void draw() {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

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

            ImGui::SliderInt("Width", &mazeWidth, 1, 100);
            ImGui::SliderInt("Height", &mazeHeight, 1, 100);
            
            ImGui::SliderFloat("Speed", &expansionSpeed, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            // Check if the expansion speed has changed
            if (prevExpansionSpeed != expansionSpeed) {
                maze.setExpandSpeed(expansionSpeed);
                prevExpansionSpeed = expansionSpeed; 
                }

            if (ImGui::Button("Reset")) {
                maze = Maze(mazeWidth, mazeHeight, expansionSpeed); 
                paused = true;
                }

            ImGui::SameLine();
            if (!paused) {
                if (ImGui::Button("Stop Expanding")) {
                    paused = true;
                    }
                maze.tick(window.getDeltaTime());
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


            ImGui::SliderFloat("Sun X", &sunPosX, -10.0f, 10.0f); 
            ImGui::SliderFloat("Sun Y", &sunPosY, 1.0f, 10.0f);
            ImGui::SliderFloat("Sun Z", &sunPosZ, -10.0f, 10.0f);

            if (ImGui::Button("Fullscreen")) {
                window.setFullscreen(!window.getFullscreen());
                }

            ImGui::End();

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glUniform3fv(glGetUniformLocation(shaderProgram, "lightDirection"), 1, glm::value_ptr(glm::vec3(sunPosX, sunPosY, sunPosZ)));
            }
   
    };
  
