
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
        std::string vertexSource, fragmentSource;
        std::string source = loadShaderSource(filepath);
        parseShaderSource(source, vertexSource, fragmentSource);

        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());
        id = linkShaders(vertexShader, fragmentShader); // Combines and links the shaders into a single shader program
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    
    void parseShaderSource(const std::string& shaderSource, std::string& vertexSource, std::string& fragmentSource) const {
        size_t vertexPos = shaderSource.find("// Vertex");
        size_t fragmentPos = shaderSource.find("// Fragment");

        if (vertexPos != std::string::npos && fragmentPos != std::string::npos) {
            vertexSource = shaderSource.substr(vertexPos, fragmentPos - vertexPos);
            fragmentSource = shaderSource.substr(fragmentPos);
        } else {
            std::cerr << "Shader section not found in source, should be a '// Vertex' and '// Fragment'" << std::endl;
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