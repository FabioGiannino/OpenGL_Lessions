#version 460 core

//è semplicemente una texture di quattro vertici, data dalla scena

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_uv;

out vec2 vert_uv_out;

void main() 
{
    gl_Position =vec4(vert_pos, 1.0f);
    vert_uv_out = vert_uv;

}