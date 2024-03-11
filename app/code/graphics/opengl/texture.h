#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include "GL/glew.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../loaders/stb_image.h"

struct Texture {
    
    int width = 0;
    int height = 0;
    unsigned int textureID = 0;
    const char* imageData = nullptr;

    Texture(int width, int height)
        : width(width), height(height) {
        glGenTextures(1, &textureID);
        resize(width, height);
    }

    Texture(const std::string& filepath, bool mipmaps = true) {
        int channels;
        stbi_set_flip_vertically_on_load(true);
        imageData = reinterpret_cast<const char*>(stbi_load(filepath.c_str(), &width, &height, &channels, 0));
        if (!imageData) {
            std::cerr << "Failed to load texture: " << filepath << std::endl;
            return;
        }
        glGenTextures(1, &textureID);
        updateBuffer(mipmaps);
    }

    ~Texture() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
        stbi_image_free(const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(imageData)));
    }

    void updateTexture(const char* newImageData, int xoffset = 0, int yoffset = 0, int subWidth = -1, int subHeight = -1) {
        if (!newImageData) {
            std::cerr << "Error: New image data is empty." << std::endl;
            return;
        }

        int defaultSubWidth = (subWidth == -1) ? width : subWidth;
        int defaultSubHeight = (subHeight == -1) ? height : subHeight;

        int targetWidth = (defaultSubWidth < width - xoffset) ? defaultSubWidth : (width - xoffset);
        int targetHeight = (defaultSubHeight < height - yoffset) ? defaultSubHeight : (height - yoffset);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, targetWidth, targetHeight, GL_RGB, GL_UNSIGNED_BYTE, newImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void updateBuffer(bool mipmaps = false) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        if (mipmaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void resize(int newWidth, int newHeight) {
        width = newWidth;
        height = newHeight;
        int dataSize = width * height * 3;
        delete[] imageData;
        imageData = new char[dataSize];
        updateBuffer(false);
    }

    void bind(unsigned int texture_unit) const {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void loadFromBitmapFile(const std::string& filepath) {
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
        delete[] imageData;
        imageData = new char[imageSize];
        file.seekg(54);
        file.read(const_cast<char*>(imageData), imageSize);
        if (imageData == nullptr) {
            std::cerr << "Error: Failed to read image data from file: " << filepath << std::endl;
            return;
        }
    }

    void printImageData() {
        std::cout << "Image Data:" << std::endl;
        std::stringstream ss;
        for (size_t i = 0; i < width * height * 3; ++i) {
            ss << static_cast<int>(imageData[i]) << " ";
        }
        std::cout << ss.str() << std::endl;
    }

};
