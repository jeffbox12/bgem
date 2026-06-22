#version 300 es

precision mediump float;

uniform sampler2D uAtlas; /* Single-channel (R) glyph coverage mask */
uniform vec4 uColor;

in vec2 vUV;
out vec4 fragColor;

void main()
{
    float coverage = texture(uAtlas, vUV).r;
    fragColor = vec4(uColor.rgb, uColor.a * coverage);
}
