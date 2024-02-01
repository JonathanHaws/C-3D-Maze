#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title, bool fullscreen) 
        : width(width), height(height), title(title), fullscreen(fullscreen) {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }

        GLFWmonitor* monitor = nullptr;
        if (fullscreen) {
            monitor = glfwGetPrimaryMonitor();
        }

        window = glfwCreateWindow(width, height, title, monitor, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW\n";
            return;
        }

        glfwSetWindowSizeCallback(window, window_size_callback);
        window_size_callback(window, width, height);
    }

    ~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void tick() {
        updateDeltaTime();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    GLFWwindow* getWindow() const {
        return window;
    }

    bool isOpen() const {
        return !glfwWindowShouldClose(window);
    }

    bool input(int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    float getDeltaTime() const {
        return deltaTime;
    }

    float getAspectRatio() const {
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        return static_cast<float>(window_width) / static_cast<float>(window_height);
    }

    void setFullscreen(bool enable) {
        fullscreen = enable;
        if (enable) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            
        } else {
            glfwSetWindowMonitor(window, nullptr, 0, 0, width, height, GLFW_DONT_CARE);
            glfwSetWindowPos(window, 0, 50);
        }
    }

    bool getFullscreen() const {
        return fullscreen;
        }

private:
    GLFWwindow* window;
    int width;
    int height;
    const char* title;
    bool fullscreen;
    double lastFrameTime;
    float deltaTime;

    void updateDeltaTime() {
        double currentFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;
    }

    static void window_size_callback(GLFWwindow* window, int window_width, int window_height) {
        glViewport(0, 0, window_width, window_height);
    }
};