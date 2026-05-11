#version 300 es

precision mediump float;

uniform float uTime;
in vec2 vUV;

out vec4 fragColor;

void main()
{
    float r = 0.5 + 0.5 * sin(uTime + vUV.x * 6.2831);
    float g = 0.5 + 0.5 * sin(uTime + vUV.y * 6.2831);
    float b = 0.5 + 0.5 * sin(uTime);

    fragColor = vec4(r, g, b, 1.0);
}
