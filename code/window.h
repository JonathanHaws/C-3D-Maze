#pragma once 

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

struct Window {

    // Input
    int mouse_delta_x;
    int mouse_delta_y;
    int mouse_previous_x;
    int mouse_previous_y;
    float delta_time, previous_time;
    std::unordered_map<int, int> previous_keys;

    // Window
    float width;
    float height;
    const char* title;
    bool fullscreen;
    float aspect_ratio() { return width / height; } // not working
    GLFWwindow* GLFW_window;

    Window(int width, int height, const char* title, bool fullscreen) 
        : width(width), height(height), title(title), fullscreen(fullscreen) {
        
        if (!glfwInit()) {
            std::cout << "Failed to initialize GLFW" << std::endl;
            exit(-1);
        }

        GLFW_window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!GLFW_window) {
            std::cout << "Failed to create window" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        glfwMakeContextCurrent(GLFW_window);

        if (glewInit() != GLEW_OK) {
            std::cout << "Failed to initialize GLEW" << std::endl;
            exit(-1);
        }

        glfwSetWindowUserPointer(GLFW_window, this);
        glViewport(0, 0, width, height);

        set_fullscreen(fullscreen);
    }
 
    ~Window() {
        glfwTerminate();
    }

    void close() {
        delete this; 
    }

    bool input(int key) {
        return glfwGetKey(GLFW_window, key) == GLFW_PRESS;
    }

    bool input_released(int key) {
        if (glfwGetKey(GLFW_window, key) != GLFW_RELEASE) { return false; }
        if (!previous_keys[key]) { return false; }
        previous_keys[key] = false;
        return true;
    }

    void poll_events() {
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
            previous_keys[key] = glfwGetKey(GLFW_window, key);
        }

        calculate_delta_time();
        calculate_mouse_delta();
        glfwPollEvents();  
    }

    bool is_open() {
        return !glfwWindowShouldClose(GLFW_window);
    }

    bool is_fullscreen() {
        return fullscreen;
    }

    void set_fullscreen(bool new_fullscreen) {
        fullscreen = new_fullscreen;
        if (new_fullscreen) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(GLFW_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(GLFW_window, nullptr, 0, 50, 1920, 1080, GLFW_DONT_CARE);
        }
    }

    void swap_buffers() {
        glfwSwapBuffers(GLFW_window);
    }

    float get_time() {
        return glfwGetTime();
    }

    void window_size_callback(GLFWwindow* window, int new_width, int new_height) {
        glViewport(0, 0, new_width, new_height);
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (win) {
            win->width = new_width;
            win->height = new_height;
        }
    }
    
    void calculate_delta_time() {
        float current_time = glfwGetTime();
        delta_time = current_time - previous_time;
        previous_time = current_time;
    }

    void calculate_mouse_delta() {
        double x, y;
        glfwGetCursorPos(GLFW_window, &x, &y);
        mouse_delta_x = x - mouse_previous_x;
        mouse_delta_y = y - mouse_previous_y;
        mouse_previous_x = x;
        mouse_previous_y = y;
    }

};