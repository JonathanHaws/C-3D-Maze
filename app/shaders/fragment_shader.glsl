#version 330 core

uniform vec3 lightDirection; // Uniform variable for light direction

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D textureSampler;
const float ambientStrength = 0.3; // Adjust ambient strength
const float diffuseStrength = 0.8; // Adjust diffuse strength

void main() {
    vec3 lightDir = normalize(lightDirection);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 ambient = ambientStrength * texture(textureSampler, TexCoord).rgb;
    vec3 diffuse = diffuseStrength * diff * texture(textureSampler, TexCoord).rgb;
    // Combine ambient and diffuse lighting for final color
    vec3 result = ambient + diffuse;
    // Output the final color
    FragColor = vec4(result, 1.0);
}