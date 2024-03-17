#pragma once 

struct Sky {
    
    glm::vec3 sun = glm::vec3(0.1, 1.0, 0.1); 
    glm::vec3 skyColor = glm::vec3(0.529f, 0.676f, 0.701f); 
    glm::vec3 objectColor = glm::vec3(1.000f, 1.000f, 1.000f);
    glm::vec3 ambientColor = glm::vec3(0.016f, 0.067f, 0.074f);
    glm::vec3 lightColor = glm::vec3(1.000f, 0.847f, 0.775f);

    Sky(glm::vec3 sun, glm::vec3 skyColor, glm::vec3 objectColor, glm::vec3 ambientColor, glm::vec3 lightColor)
        : sun(sun), skyColor(skyColor), objectColor(objectColor), ambientColor(ambientColor), lightColor(lightColor) {
        }

    void draw() {
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        }

    void bind(Shader* shader) {
        shader->setVec3("lightDirection", sun.x, sun.y, sun.z);
        shader->setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        shader->setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);
        shader->setVec3("ambientColor", ambientColor.x, ambientColor.y, ambientColor.z);
        shader->setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        }

    };
