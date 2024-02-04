#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
    public:
        Camera(const glm::vec3& initialPosition = glm::vec3(0.0f, 20.0f, -40.0f),
                const glm::vec3& initialTarget = glm::vec3(0.0f, 0.0f, -20.0f),
                const glm::vec3& initialUp = glm::vec3(0.0f, 1.0f, 0.0f),
                float fov = 80.0f,
                float aspectRatio = 1280.0f / 720.0f,
                float nearPlane = 0.1f,
                float farPlane = 1000.0f)
                : position(initialPosition), target(initialTarget), up(initialUp), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {}
                
        void draw(unsigned int VAO, unsigned int EBO, int vertexCount, float x, float y, float z, unsigned int textureID, unsigned int shaderProgram) const {

            glUseProgram(shaderProgram);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
            glm::mat4 viewMatrix = glm::lookAt(position, target, up);
            glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
            glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0); 

            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
            glUniformMatrix3fv(glGetUniformLocation(shaderProgram, "normalMatrix"), 2, GL_FALSE, glm::value_ptr(normalMatrix));
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
            
            glBindVertexArray(0);
            }

        float get_pitch() const {
            glm::vec3 direction = glm::normalize(target - position);
            return glm::degrees(asin(direction.y));
            }

        float get_yaw() const {
            glm::vec3 direction = glm::normalize(target - position);
            float yaw = atan2(direction.z, direction.x);
            if (yaw < 0) {
                yaw += 2 * glm::pi<float>(); // Ensure yaw is positive
            }
            return glm::degrees(yaw);
            }

        void setAspectRatio(float newAspectRatio) {
                aspectRatio = newAspectRatio;
            }

        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;
        };