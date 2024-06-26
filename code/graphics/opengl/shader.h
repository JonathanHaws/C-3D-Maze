#pragma once

struct Shader {
    unsigned int id;
    
    ~Shader() { glDeleteProgram(id); }
    
    void bind() const { glUseProgram(id); }
    
    void unbind() const { glUseProgram(0); }
 
    void setInt(const char* name, int value) const {
        glUniform1i(glGetUniformLocation(id, name), value);
    }
    
    void setFloat(const char* name, float value) const {
        glUniform1f(glGetUniformLocation(id, name), value);
    }
    
    void setVec3(const char* name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(id, name), x, y, z);
    }
    
    void setMat4(const char* name, const glm::mat4& matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    Shader(const std::string& filepath) {
        std::string vertexSource, fragmentSource, geometrySource; // Add geometrySource
        std::string source = loadShaderSource(filepath);
        parseShaderSource(source, vertexSource, fragmentSource, geometrySource); // Pass geometrySource

        std::vector<unsigned int> sub_shaders;
        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());
        unsigned int geometryShader = compileShader(GL_GEOMETRY_SHADER, geometrySource.c_str()); // Compile geometry shader
        sub_shaders.push_back(vertexShader);
        sub_shaders.push_back(fragmentShader);
        sub_shaders.push_back(geometryShader); // Add geometry shader to sub_shaders
        //clear_gl_errors();
        id = linkShaders(filepath, sub_shaders); // Link geometry shader
        //check_gl_errors();
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(geometryShader); 
    }
    
    void parseShaderSource(const std::string& shaderSource, std::string& vertexSource, std::string& fragmentSource, std::string& geometrySource) const {
        size_t vertexPos = shaderSource.find("// Vertex");
        size_t geometryPos = shaderSource.find("// Geometry");
        size_t fragmentPos = shaderSource.find("// Fragment");

        if (vertexPos != std::string::npos && geometryPos != std::string::npos && fragmentPos != std::string::npos) {
            // Extract substrings for each shader type
            vertexSource = shaderSource.substr(vertexPos, geometryPos - vertexPos);
            geometrySource = shaderSource.substr(geometryPos, fragmentPos - geometryPos);
            fragmentSource = shaderSource.substr(fragmentPos); // Extract fragment shader source

              // Print out the source code for verification
            // std::cout << "Vertex Shader Source:\n" << vertexSource << std::endl;
            // std::cout << "Geometry Shader Source:\n" << geometrySource << std::endl;
            // std::cout << "Fragment Shader Source:\n" << fragmentSource << std::endl;
        } else {
            std::cerr << "Shader section not found in source, should be '// Vertex', '// Geometry', and '// Fragment'" << std::endl;
        }
    }
    
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
    
    unsigned int linkShaders(const std::string& path, const std::vector<unsigned int>& shaderPrograms) const {
        unsigned int programID = glCreateProgram();
        
        for (unsigned int shader : shaderPrograms) {
            glAttachShader(programID, shader);
            }
        
        glLinkProgram(programID);

        int success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cerr << "Shader program linking failed for " << path << ":\n" << infoLog << std::endl;
        }

        return programID;
    }
};