
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
layout(triangle_strip, max_vertices = 30) out; // Keep as low as possible

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

void emit(vec3 vertex, vec2 texCoord, vec3 normal) {
    gl_Position = Projection * View * Model * vec4(vertex, 1.0);
    TexCoordNew = texCoord;
    NormalNew = normal;
    EmitVertex();
    }

void tri(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz) {
    float scale = 2.0;
    float xoffset = float(mazeWidth) / 2.0;
    float zoffset = float(mazeHeight) / 2.0;
    ax -= xoffset; az -= zoffset;
    bx -= xoffset; bz -= zoffset;
    cx -= xoffset; cz -= zoffset;
    ax *= scale; ay *= scale; az *= scale;
    bx *= scale; by *= scale; bz *= scale;
    cx *= scale; cy *= scale; cz *= scale;

    vec3 v1 = vec3(cx - ax, cy - ay, cz - az); // Calculate Normal
    vec3 v2 = vec3(bx - ax, by - ay, bz - az); // Note the swapped order
    vec3 normal = normalize(cross(v1, v2));    // Note the swapped order
    
    if (ay == by && by == cy) {
        // Vertical surface, use xz coordinates for UV
        emit(vec3(ax, ay, az), vec2(ax, az), normal);
        emit(vec3(bx, by, bz), vec2(bx, bz), normal);
        emit(vec3(cx, cy, cz), vec2(cx, cz), normal);
    } else if (ax == bx && bx == cx) {
        // Horizontal surface, use yz coordinates for UV
        emit(vec3(ax, ay, az), vec2(ay, az), normal);
        emit(vec3(bx, by, bz), vec2(by, bz), normal);
        emit(vec3(cx, cy, cz), vec2(cy, cz), normal);
    } else {
        // Diagonal surface, use xy coordinates for UV
        emit(vec3(ax, ay, az), vec2(ax, ay), normal);
        emit(vec3(bx, by, bz), vec2(bx, by), normal);
        emit(vec3(cx, cy, cz), vec2(cx, cy), normal);
    }
    EndPrimitive();
    }

void main() {
    int x = instance[0] % mazeWidth;
    int z = instance[0] / mazeWidth;

    vec2 texCoord = vec2((float(x) + 0.5) / float(mazeWidth), (float(z) + 0.5) / float(mazeHeight));
    vec4 texColor = texture(corridorsTexture, texCoord);
    if (texColor.r == 0) { return; }

    tri (x    , 0, z, x, 1, z, x + 1, 0, z); // South
    tri (x + 1, 0, z, x, 1, z, x + 1, 1, z);

    tri (x + 1, 0, z, x + 1, 1, z, x + 1, 0, z + 1); // East 
    tri (x + 1, 0, z + 1, x + 1, 1, z, x + 1, 1, z + 1);   

    tri (x, 0, z + 1, x + 1, 1, z + 1, x + 1, 0, z + 1); // North 
    tri (x, 0, z + 1, x, 1, z + 1, x + 1, 1, z + 1);    

    tri (x, 0, z + 1, x, 1, z + 1, x, 0, z); // West
    tri (x, 0, z, x, 1, z + 1, x, 1, z);   

    tri (x, 1, z, x + 1, 1, z, x + 1, 1, z + 1); // Top
    tri (x, 1, z, x + 1, 1, z + 1, x, 1, z + 1);    

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