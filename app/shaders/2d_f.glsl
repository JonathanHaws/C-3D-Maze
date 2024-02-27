#version 330 core

out vec4 FragColor;

in vec2 TexCoord; 

uniform sampler2D texture_diffuse1; 
uniform vec3 objectColor; 

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    FragColor = texColor;
}