#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal; // Receive the normal vector from the vertex shader

uniform sampler2D texture_diffuse1;
uniform vec3 objectColor;    // Uniform for object color
uniform vec3 ambientColor;   // Uniform for ambient color
uniform vec3 lightColor;     // Uniform for light color
uniform vec3 lightDirection;

void main()
{
    vec3 ambient = ambientColor * objectColor;
    vec3 norm = normalize(Normal); // Use the normal vector passed from the vertex shader
    vec3 lightDir = normalize(lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * objectColor);
    vec3 result = (ambient + diffuse) * texture(texture_diffuse1, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}