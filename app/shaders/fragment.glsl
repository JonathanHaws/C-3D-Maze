#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform vec3 objectColor;    
uniform vec3 ambientColor;   
uniform vec3 lightColor;     
uniform vec3 lightDirection;

void main()
{
    vec3 ambient = ambientColor * objectColor;
    vec3 norm = normalize(Normal); // Use the normal vector passed from the vertex shader
    vec3 lightDir = normalize(lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * objectColor);
    vec3 finalColor = (ambient + diffuse) * texture(texture_diffuse1, TexCoord).rgb;
     
    FragColor = vec4(finalColor, 1.0);
}