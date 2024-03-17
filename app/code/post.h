#pragma once

struct Post {
    
    float sunX = 0.1f;
    float sunY = 1.0f;
    float sunZ = 0.1f;   
    glm::vec3 skyColor = glm::vec3(0.529f, 0.676f, 0.701f); 
    glm::vec3 ambientColor = glm::vec3(0.016f, 0.067f, 0.074f);
    glm::vec3 lightColor = glm::vec3(1.000f, 0.847f, 0.775f);
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
    Framebuffer framebuffer = Framebuffer(1920, 1080);

    void bind() {
        framebuffer.bind();
        }

    void draw() {
        // postShader.bind();
        // postShader.setFloat("exposure", exposure);
        // postShader.setFloat("gamma", gamma);
        // postShader.setInt("fog", fog);
        // postShader.setFloat("fog_distance", fog_distance);
        // postShader.setFloat("fog_falloff", fog_falloff);
        // postShader.setVec3("fog_color", fog_color.x, fog_color.y, fog_color.z);
        // postShader.setInt("blur", blur);
        // postShader.setInt("blurRadius", blurRadius);
        // postShader.setInt("depthBuffer", depthBuffer);
        // postShader.setInt("ambientOcclusion", ambientOcclusion);
        // postShader.setInt("occlusionBuffer", occlusionBuffer);
        // postShader.setInt("occlusionRadius", occlusionRadius);
        // postShader.setFloat("occlusionThreshold", occlusionThreshold);
        // postShader.setFloat("occlusionStrength", occlusionStrength);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, framebuffer.color_texture);
        // glUniform1i(glGetUniformLocation(postShader.id, "colorTexture"), 0);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, framebuffer.depth_texture);
        // glUniform1i(glGetUniformLocation(postShader.id, "depthTexture"), 1);

        // quad.draw();

        }

    };
    