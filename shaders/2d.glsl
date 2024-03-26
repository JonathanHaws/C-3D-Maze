
// Vertex
#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal; 
out vec2 TexCoord; 
uniform mat4 Model;       
uniform mat4 View;        
uniform mat4 Projection;  
void main() {
    mat4 MV = View * Model;
    gl_Position = Projection * MV * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    }

// Geometry 
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec2 TexCoord[];
out vec2 TexCoordNew;
void main() {
    for (int i = 0; i < gl_in.length(); ++i) {
        gl_Position = gl_in[i].gl_Position;
        TexCoordNew = TexCoord[i]; 
        EmitVertex();
        }
    EndPrimitive();
    }

// Fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoordNew; 
uniform sampler2D texture_diffuse1; 
uniform vec3 objectColor;
void main() {
    vec4 texColor = texture(texture_diffuse1, TexCoordNew);
    FragColor = texColor;
    }
