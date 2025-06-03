#version 460 core       

out vec4 FragColor;
in vec2 vert_pos_out; // o gl_FragCoord.xy

uniform float base_color;

float random(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main()
{
    // scala per rendere il noise visibile
    float n = random(vert_pos_out * base_color);
    FragColor = vec4(vec3(n), 1.0);
}