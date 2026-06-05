#version 300 es

precision mediump float;

uniform sampler2D u_layer[5];

in  vec2 v_uv;
out vec4 frag_color;

void main()
{
    /* Sample all five layers bottom-to-top */
    vec4 l0 = texture(u_layer[0], v_uv);  /* background     */
    vec4 l1 = texture(u_layer[1], v_uv);  /* application    */
    vec4 l2 = texture(u_layer[2], v_uv);  /* ui             */
    vec4 l3 = texture(u_layer[3], v_uv);  /* overlay        */
    vec4 l4 = texture(u_layer[4], v_uv);  /* notification   */

    /* Composite each layer over the result below it, bottom-to-top */
    vec3 result = l0.rgb;
    result = mix(result, l1.rgb, l1.a);
    result = mix(result, l2.rgb, l2.a);
    result = mix(result, l3.rgb, l3.a);
    result = mix(result, l4.rgb, l4.a);

    frag_color = vec4(result, 1.0);
}
