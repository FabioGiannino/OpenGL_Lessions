#version 460 core

out vec4 frag_color;

layout (binding = 0) uniform sampler2D scene_texture;

in vec2 vert_uv_out;


void main() 
{
    //frag_color = texture(scene_texture, vert_uv_out);

    vec4 color = texture(scene_texture, vert_uv_out);
    float avg = (color.r + color.g + color.b)/5.0;
    frag_color = vec4(avg, avg, avg, 1.0f);

}