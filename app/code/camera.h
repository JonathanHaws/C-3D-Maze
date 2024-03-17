#pragma once
#include <libs/glm/glm.hpp>
#include <libs/glm/gtc/type_ptr.hpp>

struct Camera {
    
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float fov = 80.0f;
    float aspectRatio = 1280.0f / 720.0f;
    float nearPlane = 0.1f;
    float farPlane = 100000.0f;

    void bind(Shader* shader) {
        shader->setMat4("Model",  glm::mat4(1.0f));
        shader->setMat4("View", viewMatrix());
        shader->setMat4("Projection", projectionMatrix());  
        }
   
    float pitch() { 
        return glm::degrees(asin(glm::normalize(target - position).y)); 
        }
    
    float yaw() { 
        float yaw = atan2(glm::normalize(target - position).z, glm::normalize(target - position).x);
        if (yaw < 0) { yaw += 2 * glm::pi<float>(); }
        return glm::degrees(yaw);
        }
    
    glm::mat4 viewMatrix() {
        return glm::lookAt(position, target, up);
        }
    
    glm::mat4 projectionMatrix(bool isPerspective = true) {
        if (isPerspective) {
            return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        } else {
            float orthoLeft = -2.0f;
            float orthoRight = 2.0f;
            float orthoBottom = -2.0f;
            float orthoTop = 2.0f;
            
            float halfWidth = (orthoRight - orthoLeft) * 0.5f;
            float halfHeight = (orthoTop - orthoBottom) * 0.5f;
            
            float aspect = aspectRatio;
            if (aspect >= 1.0f) {
                // Adjust width
                halfWidth *= aspect;
            } else {
                // Adjust height
                halfHeight /= aspect;
            }
            
            orthoLeft = -halfWidth;
            orthoRight = halfWidth;
            orthoBottom = -halfHeight;
            orthoTop = halfHeight;
            
            return glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);
        }
        }
    
    void set_position(float x, float y, float z) {
        position = glm::vec3(x, y, z);
        }
    
    void set_target(float x, float y, float z) {
        target = glm::vec3(x, y, z);
        }

    Camera(float fov, float aspectRatio, float nearPlane, float farPlane) 
        : fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {
        }

    };