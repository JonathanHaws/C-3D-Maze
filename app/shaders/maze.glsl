
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
layout(triangle_strip, max_vertices = 50) out; // Keep as low as possible

in vec2 TexCoord[];
in vec3 Normal[];
in int instance[];

uniform int mazeWidth;
uniform int mazeDepth;
uniform float mazeHeight;
uniform float mazeBreadth;
uniform float mazeScale;
uniform sampler2D mazeTexture;
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
    float scale = mazeScale;
    float xoffset = float(mazeWidth) / 2.0;
    float zoffset = float(mazeDepth) / 2.0;
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

bool path(float tx, float ty) {
    float cellSize = 1.0 / float(mazeWidth);
    if (tx < 0.0 || tx >= float(mazeWidth) || ty < 0.0 || ty >= float(mazeDepth)) { return true; } 
    return texture(mazeTexture, vec2((tx + 0.5) * cellSize, (ty + 0.5) * cellSize)).r == 0;
    }

void wall(float x1, float z1, float x2, float z2) {
    tri(x1, 0.0, z1, x1, mazeHeight, z1, x2, 0.0, z2);
    tri(x2, 0.0, z2, x1, mazeHeight, z1, x2, mazeHeight, z2);
    }

void roof(float x1, float z1, float x2, float z2) {
    tri(x1, mazeHeight, z1, x2, mazeHeight, z1, x2, mazeHeight, z2);
    tri(x1, mazeHeight, z1, x2, mazeHeight, z2, x1, mazeHeight, z2);
    }

void main() {
    float x = instance[0] % mazeWidth;
    float z = instance[0] / mazeWidth;

    if (path(x, z)) { return; }

    if (!path(x,z-1) && !path(x-1,z) && !path(x-1,z-1)){ //roof
        roof(x,z,x+1,z+1);
        //return;
        }

    if (!path(x,z-1) && path(x-1,z)) { // Vertical Wall
        wall(x + (1 - mazeBreadth), z, x + (1 - mazeBreadth), z + 1);
        wall(x + (1 - mazeBreadth), z, x+1, z);
        wall(x + (1 - mazeBreadth) , z+1, x+1, z +1);
        wall(x + 1, z, x+1, z +1);
        roof(x + (1 - mazeBreadth), z, x+1, z+1);
        return;   
        }

    if (path(x,z-1) && !path(x-1,z)) { // Horizontal Wall
        wall(x, z+ (1 - mazeBreadth), x+1, z+ (1 - mazeBreadth));
        wall(x, z+ (1 - mazeBreadth), x, z+1);
        roof(x, z+(1 - mazeBreadth), x+1, z+1);
        wall(x+1, z+ (1 - mazeBreadth), x+1, z+1);
        wall(x, z+1, x+1, z+1);
        return;
        }

    if (path(x,z-1) && path(x-1,z)) { // Anti Corner
        wall(x+ (1 - mazeBreadth), z+ (1 - mazeBreadth), x + (1 - mazeBreadth), z+1);
        wall(x+ (1 - mazeBreadth), z+ (1 - mazeBreadth), x + 1, z +(1 - mazeBreadth));
        wall(x+ 1, z+(1 - mazeBreadth), x+1, z+1);
        wall(x+ (1 - mazeBreadth), z+1, x+1, z+1);
        roof(x+ (1 - mazeBreadth), z+ (1 - mazeBreadth), x+1, z+1);
        return;
        }

    if (!path(x,z-1) && !path(x-1,z)) { // Corner

        roof(x+1, z+1, x, z+ (1 - mazeBreadth));
        roof(x+(1 - mazeBreadth), z, x + 1, z + (1 - mazeBreadth));

        wall(x, z + (1 - mazeBreadth), x+ (1 - mazeBreadth), z + (1 - mazeBreadth));
        wall(x + (1 - mazeBreadth), z, x + (1 - mazeBreadth), z+(1 - mazeBreadth));

        wall(x, z+1, x+1, z+1);
        wall(x+1, z, x+1, z+1);
        return;
        }

    }

// Fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoordNew;
in vec3 NormalNew;  

uniform sampler2D bricksDiffuseTexture;
uniform sampler2D bricksNormalTexture; 

uniform vec3 ambientColor;   
uniform vec3 lightColor;     
uniform vec3 lightDirection;
uniform float brickSize;

void main() {


    vec3 diffuseColor = texture(bricksDiffuseTexture, TexCoordNew * brickSize).rgb;

    // Normalize normal vector passed from vertex shader
    vec3 norm = normalize(NormalNew);

    // Sample normal texture to get perturbed normal
    vec3 normalMap = texture(bricksNormalTexture, TexCoordNew * brickSize).rgb;

    // Transform normal map values from [0, 1] range to [-1, 1]
    vec3 perturbedNormal = normalize(normalMap * 2.0 - 1.0);

    // Combine perturbed normal with vertex normal
    vec3 finalNormal = normalize(perturbedNormal + norm);

    // Compute diffuse lighting
    float diffuseIntensity = max(dot(finalNormal, normalize(lightDirection)), 0.0);

    // Final color with light color taken into account
    vec3 finalColor = ambientColor + diffuseColor * diffuseIntensity * lightColor;

    FragColor = vec4(finalColor, 1.0);
    }
