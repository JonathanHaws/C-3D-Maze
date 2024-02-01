#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>

class Shader {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath) {
            unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, loadShaderSource(vertexPath));
            unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, loadShaderSource(fragmentPath));
            id = linkShaders(vertexShader, fragmentShader); // Combines and links the shaders into a single shader program
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            }

        ~Shader() { 
            glDeleteProgram(id); 
            }

        void Bind() const {
            glUseProgram(id);
            }

        void Unbind() const {
            glUseProgram(0);
            }

        unsigned int getID() const {
            return id;
            }
    private:
        unsigned int id;
        const char* loadShaderSource(const std::string& filepath) const {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open shader file: " << filepath << std::endl;
                return nullptr;
                }

            std::stringstream buffer;
            buffer << file.rdbuf();

            std::string sourceString = buffer.str();
            char* source = new char[sourceString.size() + 1];
            std::copy(sourceString.begin(), sourceString.end(), source);
            source[sourceString.size()] = '\0'; // Null-terminate the string

            return source;
            }

        unsigned int compileShader(unsigned int type, const char* source) const {
            unsigned int shaderID = glCreateShader(type);
            glShaderSource(shaderID, 1, &source, nullptr);
            glCompileShader(shaderID);

            int success;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
                std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
                }

            return shaderID;
            }

        unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader) const {
            unsigned int programID = glCreateProgram();
            glAttachShader(programID, vertexShader);
            glAttachShader(programID, fragmentShader);
            glLinkProgram(programID);

            int success;
            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(programID, 512, nullptr, infoLog);
                std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
            }

            return programID;
        }
    };