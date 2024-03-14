
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

void emit(vec3 vertex, vec2 texCoord, vec3 normal) {
    gl_Position = Projection * View * Model * vec4(vertex, 1.0);
    TexCoordNew = texCoord;
    NormalNew = normal;
    EmitVertex();
    }

void tri(float ax, float ay, float az, float au, float av, float bx, float by, float bz, float bu, float bv, float cx, float cy, float cz, float cu, float cv, float nx, float ny, float nz) {
    float scale = 8.0;
    float xoffset = float(mazeWidth) / 2.0;
    float zoffset = float(mazeHeight) / 2.0;
    ax -= xoffset; az -= zoffset;
    bx -= xoffset; bz -= zoffset;
    cx -= xoffset; cz -= zoffset;
    ax *= scale; ay *= scale; az *= scale;
    bx *= scale; by *= scale; bz *= scale;
    cx *= scale; cy *= scale; cz *= scale;
    
    emit(vec3(ax, ay, az), vec2(au, av), vec3(nx, ny, nz));
    emit(vec3(bx, by, bz), vec2(bu, bv), vec3(nx, ny, nz));
    emit(vec3(cx, cy, cz), vec2(cu, cv), vec3(nx, ny, nz));
    EndPrimitive();
    }

void main() {
    int x = instance[0] % mazeWidth;
    int z = instance[0] / mazeWidth;
    tri (x, 0, z, 0, 0, x, 1, z, 0, 1, x + 1, 0, z, 1, 0, 0, 1, 1);
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