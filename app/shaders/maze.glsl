
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
uniform int mazeWidth;
uniform int mazeHeight;
uniform sampler2D corridorsTexture;
void main() {
    int instanceIndex = gl_InstanceID;
    int posX = instanceIndex % mazeWidth;
    int posZ = instanceIndex / mazeWidth;  

    vec2 texCoord = vec2((float(posX) + 0.5) / float(mazeWidth), (float(posZ) + 0.5) / float(mazeHeight));
    vec4 texColor = texture(corridorsTexture, texCoord);
    if (texColor.r == 0) {
        gl_Position = vec4(0.0);
        return;
        }
   
    mat4 instancedModel = mat4(1.0);
    instancedModel[3].x = ((posX - mazeWidth / 2) * 2.0);
    instancedModel[3].z = ((posZ - mazeHeight / 2) * 2.0);

    mat4 MV = View * instancedModel;
    gl_Position = Projection * MV * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(Model))) * aNormal;
    }

// Geometry (Default: Pass-through)
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