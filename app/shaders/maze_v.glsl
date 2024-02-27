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

    vec4 texColor = texture(corridorsTexture, vec2(float(posX) / float(mazeWidth), float(posZ) / float(mazeHeight)));
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