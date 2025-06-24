#version 460 core

out vec4 frag_color;

layout (binding = 0) uniform sampler2D storm_tex;

in vec2 vert_uv_out;

void main() 
{
    frag_color = texture(storm_tex, vert_uv_out);
}