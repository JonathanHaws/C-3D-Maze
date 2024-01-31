#pragma once

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Texture {
    private:
        std::string filepath;
        std::vector<unsigned char> imageData;
        int width, height;
        unsigned int textureID; // Texture ID
    public:
        Texture(const std::string& filepath) : filepath(filepath), width(0), height(0), textureID(0) {
            // Load image data from bitmap file
            std::ifstream file(filepath, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Error: Unable to open file: " << filepath << std::endl;
                return;
                }
            file.seekg(18);
            file.read(reinterpret_cast<char*>(&width), sizeof(width));
            file.read(reinterpret_cast<char*>(&height), sizeof(height));
            if (width <= 0 || height <= 0) {
                std::cerr << "Error: Invalid image dimensions in file: " << filepath << std::endl;
                return;
                }
            size_t imageSize = width * height * 3;
            imageData.resize(imageSize);
            file.seekg(54);
            file.read(reinterpret_cast<char*>(imageData.data()), imageSize);
            if (imageData.empty()) {
                std::cerr << "Error: Failed to read image data from file: " << filepath << std::endl;
                return;
                }

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData.data());
            glGenerateMipmap(GL_TEXTURE_2D);
            }

        ~Texture() {
            if (textureID != 0) {
                glDeleteTextures(1, &textureID);
                textureID = 0;
                }
            }

        void printImageData() const {
            std::cout << "Image Data:" << std::endl;
            for (size_t i = 0; i < imageData.size(); ++i) {
                std::cout << static_cast<int>(imageData[i]) << " ";
                }
            std::cout << std::endl;
            }


        int getWidth() const { return width; }
        int getHeight() const { return height; }
        unsigned int getID() const { return textureID; }
        const std::vector<unsigned char>& getImageData() const { return imageData; }
    };


