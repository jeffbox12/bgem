#version 300 es

in vec2 aPos; /* Local glyph-quad position, pixels, relative to this
                 object's own pen origin (top-left of its line box,
                 Y increasing downward) */
in vec2 aUV;  /* Atlas UV */

uniform vec2 uPosition;   /* Object's top-left anchor, pixels, Y down */
uniform vec2 uScreenSize; /* Compositor's internal canvas size, pixels */

out vec2 vUV;

void main()
{
    vec2 screenPos = aPos + uPosition;

    /* Pixel space (origin top-left, Y down) -> NDC (origin center, Y up) */
    vec2 ndc = vec2(
        (screenPos.x / uScreenSize.x) * 2.0 - 1.0,
        1.0 - (screenPos.y / uScreenSize.y) * 2.0
    );

    vUV = aUV;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
