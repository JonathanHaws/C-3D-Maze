#pragma once 

struct Sky {
    
    glm::vec3 sun = glm::vec3(0.02f, 0.365f, 0.167f); 
    glm::vec3 skyColor = glm::vec3(0.529f, 0.676f, 0.701f); 
    glm::vec3 objectColor = glm::vec3(1.000f, 1.000f, 1.000f);
    glm::vec3 ambientColor = glm::vec3(0.016f, 0.067f, 0.074f);
    glm::vec3 lightColor = glm::vec3(1.000f, 0.847f, 0.775f);

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
