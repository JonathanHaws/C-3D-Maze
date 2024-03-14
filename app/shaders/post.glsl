
// Vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal; 
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos * 1, 1.0); // Scale down the position by half
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
in vec2 TexCoordNew;
out vec4 FragColor;
uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform bool fog;
uniform float fog_distance;
uniform float fog_falloff;
uniform vec3 fog_color;
uniform bool depthBuffer;
uniform bool blur;
uniform int blurRadius;
uniform bool ambientOcclusion;
uniform bool occlusionBuffer;
uniform int occlusionRadius;
uniform float occlusionThreshold;
uniform float occlusionStrength;
uniform float exposure;
uniform float gamma;
void main() {
    
    float depth = pow(texture(depthTexture, TexCoordNew).r, 1000);
    vec3 finalColor = texture(colorTexture, TexCoordNew).rgb;  

    finalColor *= exposure;

    if (depthBuffer) {
        FragColor = vec4(vec3(depth), 1.0);
        return;
        }
  
    if (ambientOcclusion) { 
        int width = int(occlusionRadius) * 2 + 1;
        vec2 texelSize = 1.0 / textureSize(depthTexture, 0);
        float centerDepth = pow(texture(depthTexture, TexCoordNew).r, 1000);
        
        float occlusion = 0.0;

        for (int i = 0; i < width * width; ++i) {
            int x = i % width - int(occlusionRadius);
            int y = i / width - int(occlusionRadius);
            vec2 sampleCoord = TexCoordNew + vec2(x, y) * texelSize;
            sampleCoord = clamp(sampleCoord, vec2(0.0), vec2(1.0)); // Fix Borders
            float sampleDepth = pow(texture(depthTexture, sampleCoord).r, 1000); 
            
            float depthDifference = centerDepth - sampleDepth;

            float distance = length(vec2(x, y)); // Euclidean distance
            float weight = (1.0 - smoothstep(0.0, occlusionThreshold, abs(depthDifference))) / (distance + 1.0); // Weighting based on both depth difference and distance

            // Accumulate occlusion weighted by depth difference and distance
            occlusion += depthDifference * weight;
            }
        
        occlusion = 1 - (occlusion * occlusionStrength);

        finalColor *= occlusion; // Invert occlusion

        if (occlusionBuffer) {
            FragColor = vec4(vec3(occlusion), 1.0);
            return;
            }
        }

    if (blur) {
        int width = int(blurRadius) * 2 + 1;
        vec2 texelSize = 1.0 / textureSize(colorTexture, 0);
        vec3 blurColor = vec3(0.0);
        for (int i = 0; i < width * width; ++i) {
            int x = i % width - int(blurRadius);
            int y = i / width - int(blurRadius);
            vec2 sampleCoord = TexCoordNew + vec2(x, y) * texelSize;
            sampleCoord = clamp(sampleCoord, 0.0, 1.0); // Clamp both x and y coordinates
            blurColor += texture(colorTexture, sampleCoord).rgb;
            }
        finalColor = blurColor / float(width * width);
        }

    if (fog) {
        float fog_amount = step(fog_distance, depth) * (smoothstep(fog_distance, fog_distance + fog_falloff, depth));
        finalColor = mix(finalColor, fog_color, fog_amount);
        }
  
    finalColor = pow(finalColor, vec3(1.0 / gamma));

    FragColor = vec4(finalColor, 1.0);

    }
    