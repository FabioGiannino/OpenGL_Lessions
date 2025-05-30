#version 460 core       

out vec4 frag_color;    

in vec2 vert_uv_out; 

uniform sampler2D smile_tex;


void main()
{
    frag_color = texture(smile_tex, vert_uv_out);
}