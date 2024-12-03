#version 120

uniform sampler2D u_Texture;
uniform vec2 u_TextureSize;
varying vec2 TexCoord0;
const float radius = 2.0; // Adjust blur radius as needed

void main()
{
    vec2 texelSize = 1.0 / u_TextureSize;
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

    for (int x = -int(radius); x <= int(radius); ++x) {
        for (int y = -int(radius); y <= int(radius); ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            vec4 sample = texture2D(u_Texture, TexCoord0 + offset);
            float weight = exp(-(x * x + y * y) / (2.0 * radius * radius));
            color += sample * weight;
            totalWeight += weight;
        }
    }

    color = vec4(1,0,1,1);

        gl_FragColor = gl_Color * color;
        //gl_FragColor = gl_Color * centerColor;

}