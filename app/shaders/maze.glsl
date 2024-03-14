
// Vertex
#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal; 
out int instance; // Export instance variable
void main() {
    instance = gl_InstanceID;
    gl_Position = vec4(aPos, 1.0);
}

// Geometry 
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

in vec2 TexCoord[];
in vec3 Normal[];
in int instance[];

uniform int mazeWidth;
uniform int mazeHeight;
uniform sampler2D corridorsTexture;
uniform mat4 Model;
uniform mat4 View;        
uniform mat4 Projection; 

out vec2 TexCoordNew;
out vec3 NormalNew;

void main() {

    int posX = instance[0] % mazeWidth;
    int posY = instance[0] / mazeWidth;

    // Vertices of the triangle
    vec3 vertex1 = vec3(posX, posY, 0.0);
    vec3 vertex2 = vec3(posX + 1.0, posY, 0.0);
    vec3 vertex3 = vec3(posX, posY + 1.0, 0.0);

    // Perform MVP transformation
    gl_Position = Projection * View * Model * vec4(vertex1, 1.0);
    TexCoordNew = vec2(0.0, 0.0); // Example texture coordinate
    NormalNew = vec3(0.0, 0.0, 1.0); // Example normal
    EmitVertex();

    gl_Position = Projection * View * Model * vec4(vertex2, 1.0);
    TexCoordNew = vec2(1.0, 0.0); // Example texture coordinate
    NormalNew = vec3(0.0, 0.0, 1.0); // Example normal
    EmitVertex();

    gl_Position = Projection * View * Model * vec4(vertex3, 1.0);
    TexCoordNew = vec2(0.0, 1.0); // Example texture coordinate
    NormalNew = vec3(0.0, 0.0, 1.0); // Example normal
    EmitVertex();

    EndPrimitive();
}

// Fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoordNew;
in vec3 NormalNew;
uniform sampler2D texture_diffuse1;
uniform vec3 objectColor;    
uniform vec3 ambientColor;   
uniform vec3 lightColor;     
uniform vec3 lightDirection;
void main() {
    vec3 ambient = ambientColor * objectColor;
    vec3 norm = normalize(NormalNew); // Use the normal vector passed from the vertex shader
    vec3 lightDir = normalize(lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * objectColor);
    vec3 finalColor = (ambient + diffuse) * texture(texture_diffuse1, TexCoordNew).rgb;
 
    FragColor = vec4(finalColor, 1.0);
    }