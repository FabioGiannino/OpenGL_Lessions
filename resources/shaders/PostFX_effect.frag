#version 460 core

out vec4 frag_color;

layout (binding = 0) uniform sampler2D scene_texture;

in vec2 vert_uv_out;


void main() 
{
    frag_color = texture(scene_texture, vert_uv_out);

}