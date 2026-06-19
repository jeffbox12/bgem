#version 300 es

// VIBECODED FOR QUICK TESTING, NOT PART OF THE DESIGN

precision mediump float;

uniform float uTime;
in vec2 vUV;

out vec4 fragColor;

// Three ocean tones: deep trough, mid body, lifted crest.
const vec3 OCEAN_DEEP  = vec3(0.012, 0.090, 0.180);
const vec3 OCEAN_MID   = vec3(0.020, 0.250, 0.380);
const vec3 OCEAN_LIGHT = vec3(0.090, 0.430, 0.560);

void main()
{
    // Global speed. Cranked up for testing visibility (calm value is ~0.15).
    float t = uTime * 0.8;

    // Wave layers: higher frequencies + faster speeds = obvious motion.
    float w = 0.0;
    w += sin(vUV.x * 4.0 + t * 1.5);
    w += sin(vUV.y * 3.0 - t * 1.2 + 1.3);
    w += sin((vUV.x + vUV.y) * 3.5 + t * 1.8);
    w *= (1.0 / 3.0);                 // back into roughly [-1, 1]

    // Gradient base now PULSES with time (the 0.25 * sin term), so the whole
    // field visibly sweeps between deep and light. Big wave amplitude (0.45).
    float g = clamp(vUV.y * 0.6 + w * 0.45 + 0.25 * sin(t * 0.7) + 0.25,
                    0.0, 1.0);

    // Two-stop blend: deep -> mid across the lower band, mid -> light up top.
    vec3 col = mix(OCEAN_DEEP, OCEAN_MID,   smoothstep(0.00, 0.60, g));
    col      = mix(col,        OCEAN_LIGHT, smoothstep(0.55, 1.00, g));

    // Strong moving highlight so the surface clearly shimmers.
    float shimmer = sin(vUV.x * 6.0 - t * 2.5) * 0.5 + 0.5;
    col += OCEAN_LIGHT * shimmer * 0.25 * smoothstep(0.30, 1.00, g);

    fragColor = vec4(col, 1.0);
}
