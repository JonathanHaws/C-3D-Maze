#pragma once 
#include <libs/imgui/imgui.h>
#include <libs/imgui/imgui_impl_glfw.h>
#include <libs/imgui/imgui_impl_opengl3.h>

struct Editor {

    Window& window;
    Camera& camera; 
    Sky& sky;
    Finalizer& finalizer;
    Maze& maze;
    bool drawMazeTexture = false;

    Editor (Window& window, Camera& camera, Sky& sky, Finalizer& finalizer, Maze& maze): 
            window(window), camera(camera), sky(sky), finalizer(finalizer), maze(maze) {

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
            ImGui::SliderFloat("Speed", &camera.speed, 1.0f, 20.0f);
            ImGui::Checkbox("Collide", &camera.collide);
            ImGui::SliderFloat("Head Height", &camera.headheight, 0.1f, 10.0f);
            ImGui::SliderFloat("Gravity", &camera.gravity, -10.0f, 0.0f);
            }

        if (ImGui::CollapsingHeader("Maze")) {
            ImGui::SliderFloat("Height", &maze.height, 0.0, 10.0);
            ImGui::SliderFloat("Breadth", &maze.breadth, 0.0, 1.0);
            ImGui::SliderInt("Width", &maze.width, 1, 2048);
            ImGui::SliderInt("Depth", &maze.depth, 1, 2048);
            ImGui::SliderFloat("Scale", &maze.scale, 0.1f, 10.0f);
            ImGui::SliderFloat("Brick Size", &maze.brickSize, 0.1f, 10.0f);
            ImGui::SliderFloat("Expansion Speed", &maze.speed, 0.0f, 5000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
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

        if (ImGui::CollapsingHeader("Sky")) {
            ImGui::SliderFloat("Sun X", &sky.sun.x, -10.0f, 10.0f); 
            ImGui::SliderFloat("Sun Y", &sky.sun.y, 1.0f, 10.0f);
            ImGui::SliderFloat("Sun Z", &sky.sun.z, -10.0f, 10.0f);

            ImGui::ColorEdit3("Sky Color", glm::value_ptr(sky.skyColor));
            ImGui::ColorEdit3("Object Color", glm::value_ptr(sky.objectColor));
            ImGui::ColorEdit3("Ambient Color", glm::value_ptr(sky.ambientColor));
            ImGui::ColorEdit3("Light Color", glm::value_ptr(sky.lightColor));
            }

        if (ImGui::CollapsingHeader("Post Shader")) {
            ImGui::Checkbox("Depth Buffer", &finalizer.depthBuffer);
            ImGui::SliderFloat("Exposure", &finalizer.exposure, 0.0f, 10.0f);
            ImGui::SliderFloat("Gamma", &finalizer.gamma, 0.0f, 10.0f);

            ImGui::Checkbox("Fog", &finalizer.fog);
            if (finalizer.fog) { 
                ImGui::SliderFloat("Distance", &finalizer.fog_distance, 0.0f, 1.0f); 
                ImGui::SliderFloat("Falloff", &finalizer.fog_falloff, 0.0f, 1.0f);
                ImGui::ColorEdit3("Color", glm::value_ptr(finalizer.fog_color));
                }
                        
            ImGui::Checkbox("Blur", &finalizer.blur);
            if (finalizer.blur) { 
                ImGui::SliderInt("Blur Radius", &finalizer.blurRadius, 1, 10); 
                }

            ImGui::Checkbox("Ambient Occlusion", &finalizer.ambientOcclusion);
            if (finalizer.ambientOcclusion) { 
                ImGui::Checkbox("Occlusion Buffer", &finalizer.occlusionBuffer);
                ImGui::SliderInt("Radius", &finalizer.occlusionRadius, 1, 10); 
                ImGui::SliderFloat("Threshold", &finalizer.occlusionThreshold, 0.0f, 1.0f);
                ImGui::SliderFloat("Strength", &finalizer.occlusionStrength, 0.0f, 100.0f);
                }
            }
        
        if (ImGui::CollapsingHeader("Performance")) {
            ImGui::Text("FPS: %.1f", 1 / window.delta_time);
            }

        ImGui::End();

        end_frame();
        }

    };