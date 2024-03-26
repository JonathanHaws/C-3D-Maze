#pragma once

struct Finalizer {
    
    bool fog = true;
    float fog_distance = 0.9f;
    float fog_falloff = 0.15f;
    glm::vec3 fog_color = glm::vec3(0.529f, 0.676f, 0.701f);
    bool blur = false;
    int blurRadius = 3;
    bool ambientOcclusion = false;
    bool occlusionBuffer = false;
    int occlusionRadius = 2;
    float occlusionThreshold = 0.5f;
    float occlusionStrength = 1.0f;
    bool depthBuffer = false;
    float exposure = 1.0f;
    float gamma = 1.0f;
    //Framebuffer buffer = Framebuffer(1920, 1080);
    Mesh quad = Mesh("meshes/quad.obj");

    void draw(Shader* shader, Framebuffer* buffer) {

        shader->bind();
        shader->setFloat("exposure", exposure);
        shader->setFloat("gamma", gamma);
        shader->setInt("fog", fog);
        shader->setFloat("fog_distance", fog_distance);
        shader->setFloat("fog_falloff", fog_falloff);
        shader->setVec3("fog_color", fog_color.x, fog_color.y, fog_color.z);
        shader->setInt("blur", blur);
        shader->setInt("blurRadius", blurRadius);
        shader->setInt("depthBuffer", depthBuffer);
        shader->setInt("ambientOcclusion", ambientOcclusion);
        shader->setInt("occlusionBuffer", occlusionBuffer);
        shader->setInt("occlusionRadius", occlusionRadius);
        shader->setFloat("occlusionThreshold", occlusionThreshold);
        shader->setFloat("occlusionStrength", occlusionStrength);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buffer->color_texture);
        glUniform1i(glGetUniformLocation(shader->id, "colorTexture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, buffer->depth_texture);
        glUniform1i(glGetUniformLocation(shader->id, "depthTexture"), 1);

        quad.draw();

        }

    };
    