
// Vertex
#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal; 
out vec2 TexCoord; 
out vec3 Normal;
uniform mat4 Model;       
uniform mat4 View;        
uniform mat4 Projection;  
void main() {
    mat4 MV = View * Model;
    gl_Position = Projection * MV * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(Model))) * aNormal;
}

// Fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoord; 
uniform sampler2D texture_diffuse1; 
uniform vec3 objectColor;
void main() {
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    FragColor = texColor;
}
