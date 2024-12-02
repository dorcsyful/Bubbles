#version 330 core

uniform sampler2D u_Texture;

in vec2 fs_TexCoord;
out vec4 FragColor;

const float radius = 2.0; // Adjust blur radius as needed

void main()
{
    vec2 texelSize = 1.0 / textureSize(u_Texture, 0);
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

    for (int x = -int(radius); x <= int(radius); ++x) {
        for (int y = -int(radius); y <= int(radius); ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            vec4 sample = texture(u_Texture, fs_TexCoord + offset);
            float weight = exp(-(x * x + y * y) / (2.0 * radius * radius));
            color += sample * weight;
            totalWeight += weight;
        }
    }

    color /= totalWeight;
    FragColor = color;
}