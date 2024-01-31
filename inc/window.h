#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
    
    public:

        Window(int width, int height, const char* title) : width(width), height(height), title(title) {
            if (!glfwInit()) {
                std::cerr << "Failed to initialize GLFW\n";
                return;
                }

            window = glfwCreateWindow(width, height, title, nullptr, nullptr);
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

            // // Retrieve OpenGL version
            // const GLubyte* version = glGetString(GL_VERSION);
            // if (version) {
            //     std::cout << "OpenGL Version: " << version << std::endl;
            // } else {
            //     std::cerr << "Failed to retrieve OpenGL version\n";
            // }

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

        bool isOpen() const {
            return !glfwWindowShouldClose(window);
            }
        
        bool input(int key) {
            if (glfwGetKey(window, key) == GLFW_PRESS) {
                return true;
                }
            return false;
            }

        float getDeltaTime() const {
            return deltaTime;
            }

        float getAspectRatio() const {
            int window_width, window_height;
            glfwGetWindowSize(window, &window_width, &window_height);
            return static_cast<float>(window_width) / static_cast<float>(window_height);
            }

    private:
        
        GLFWwindow* window;
        int width;
        int height;
        const char* title;
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