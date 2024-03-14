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

// Geometry 
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
void main() {
    for (int i = 0; i < gl_in.length(); ++i) {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
        }
    EndPrimitive();
    }

// Fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
uniform sampler2D texture_diffuse1;
uniform vec3 objectColor;    
uniform vec3 ambientColor;   
uniform vec3 lightColor;     
uniform vec3 lightDirection;
void main() {
    vec3 ambient = ambientColor * objectColor;
    vec3 norm = normalize(Normal); // Use the normal vector passed from the vertex shader
    vec3 lightDir = normalize(lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * objectColor);
    vec3 finalColor = (ambient + diffuse) * texture(texture_diffuse1, TexCoord).rgb;
     
    FragColor = vec4(finalColor, 1.0);
    }