#version 460 core       

out vec4 frag_color;    

in vec2 vert_uv_out; 

//uniform sampler2D smile_tex;
layout (binding = 0) uniform sampler2D smile_tex;   //si collega alla texture_0 caricata in gpu

layout (binding = 1) uniform sampler2D box_tex;     //si collega alla texture_1 caricata [la woodbox ]

void main()
{
    //frag_color = texture(smile_tex, vert_uv_out);
    //frag_color = texture(box_tex, vert_uv_out);

    vec4 smile_texel = texture(smile_tex, vert_uv_out);
    vec4 box_texel = texture(box_tex, vert_uv_out);

    frag_color = mix(smile_texel, box_texel, 0.5f); //il terzo parametro è l'alpha di interpolazione. se 0, si vedrà solo lo smile, se 1 solo la box

}