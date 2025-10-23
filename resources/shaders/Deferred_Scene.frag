/*
In questa nuova versione, abbiamo che conosciamo già quali pixel accendere, di conseguenza 
anche l'alpha blending dei singoli è già stato calcolato
*/

#version 460 core 

//--Output
out vec3 frag_color;        //GL_COLOR_ATTACHMENT0
out vec3 world_normal;      //GL_COLOR_ATTACHMENT1
out vec3 world_position;    //GL_COLOR_ATTACHMENT2

//--input
layout (binding = 0) uniform sampler2D storm_tex;
in vec2 vert_uv_out;
in vec3 vert_world_normal_out;
in vec3 vert_world_position_out;

void main()
{
    frag_color = texture(storm_tex, vert_uv_out).rgb;
    world_normal = normalize(vert_world_normal_out);
    world_position = world_position;
}


