precision mediump float;

uniform float uTime;
varying vec2 vUV;

void main()
{
    float r = 0.5 + 0.5 * sin(uTime + vUV.x * 6.2831);
    float g = 0.5 + 0.5 * sin(uTime + vUV.y * 6.2831);
    float b = 0.5 + 0.5 * sin(uTime);

    gl_FragColor = vec4(r, g, b, 1.0);
}
