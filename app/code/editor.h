#pragma once 
#include <libs/imgui/imgui.h>
#include <libs/imgui/imgui_impl_glfw.h>
#include <libs/imgui/imgui_impl_opengl3.h>
#include <libs/glm/glm.hpp>
#include <graphics/graphics.h>
#include <maze.h>

struct Editor {

    Window& window;
    Camera& camera; 
    Maze& maze;
    bool drawMazeTexture = false;
    float& sunX;
    float& sunY;
    float& sunZ;
    glm::vec3& skyColor;
    glm::vec3& objectColor;
    glm::vec3& ambientColor;
    glm::vec3& lightColor;
    bool& depthBuffer;
    float& exposure;
    float& gamma;
    bool& fog;
    float& fog_distance;
    float& fog_falloff;
    glm::vec3&  fog_color;
    bool& blur;
    int& blurRadius;
    bool& ambientOcclusion;
    bool& occlusionBuffer;
    int& occlusionRadius;
    float& occlusionThreshold;
    float& occlusionStrength;

    Editor (Window& window, 
            Camera& camera, 
            Maze& maze, 
            float& sunX, float& sunY, float& sunZ, 
            glm::vec3& skyColor, glm::vec3& objectColor, glm::vec3& ambientColor, glm::vec3& lightColor, 
            bool& depthBuffer, float& exposure, float& gamma,
            bool& fog, float& fog_distance, float& fog_falloff, glm::vec3& fog_color,
            bool& blur, int& blurRadius,
            bool& ambientOcclusion, bool& occlusionBuffer, int& occlusionRadius, float& occlusionThreshold, float& occlusionStrength
            ):
            window(window), camera(camera), maze(maze), 
            sunX(sunX), sunY(sunY), sunZ(sunZ), 
            skyColor(skyColor), objectColor(objectColor), ambientColor(ambientColor), lightColor(lightColor),
            depthBuffer(depthBuffer), exposure(exposure), gamma(gamma),
            fog(fog), fog_distance(fog_distance), fog_falloff(fog_falloff), fog_color(fog_color),
            blur(blur), blurRadius(blurRadius),
            ambientOcclusion(ambientOcclusion), occlusionBuffer(occlusionBuffer), occlusionRadius(occlusionRadius), occlusionThreshold(occlusionThreshold), occlusionStrength(occlusionStrength)

            {

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
        }
    
    ~Editor() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        }
    
    void begin_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        }
    
    void end_frame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    
    void edit() {
        
        begin_frame();

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
            ImGui::SliderFloat("Speed", &maze.speed, 0.0f, 5000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            if (ImGui::Button("Reset")) {
                maze.reset();
                }
            ImGui::SameLine();
            if (ImGui::Button("Expand Once")) {
                maze.expand();
                maze.timer = 0.0f; // Reset the timer for maze expansion
                }
            ImGui::SameLine();
            if (ImGui::Button("Expand All")) {
                maze.loop = false;
                maze.expand_all();
                }

            ImGui::Checkbox("Loop", &maze.loop);
            ImGui::Checkbox("Texture", &drawMazeTexture);
            if (drawMazeTexture) {
                maze.drawTexture();
                }
            }

        if (ImGui::CollapsingHeader("Lighting")) {
            ImGui::SliderFloat("Sun X", &sunX, -10.0f, 10.0f); 
            ImGui::SliderFloat("Sun Y", &sunY, 1.0f, 10.0f);
            ImGui::SliderFloat("Sun Z", &sunZ, -10.0f, 10.0f);

            ImGui::ColorEdit3("Sky Color", glm::value_ptr(skyColor));
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
        
        if (ImGui::CollapsingHeader("Performance")) {
            ImGui::Text("FPS: %.1f", 1 / window.delta_time);
            }

        ImGui::End();

        end_frame();
        }

    };