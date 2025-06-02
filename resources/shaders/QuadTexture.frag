#version 460 core       

out vec4 frag_color;    

in vec2 vert_uv_out; 

//uniform sampler2D smile_tex;
layout (binding = 0) uniform sampler2D smile_tex;   //si collega alla texture_0 caricata in gpu

void main()
{
    frag_color = texture(smile_tex, vert_uv_out);
}