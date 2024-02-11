#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

uniform float fog_distance;
uniform float fog_falloff;
uniform vec3 fog_color;
uniform bool depthBuffer;

void main() {

    float fogDepth = pow(texture(depthTexture, TexCoord).r , fog_falloff) - (fog_distance * .01);

    
    if (depthBuffer) {

        FragColor = vec4(vec3(fogDepth), 1.0);

    } else {

        FragColor = vec4(mix(texture(colorTexture, TexCoord).rgb, fog_color, clamp(fogDepth, 0.0, 1.0)), 1.0);

        }
    }   