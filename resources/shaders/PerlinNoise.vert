#version 460 core      

layout (location = 0) in vec3 vert_pos; 
layout (location = 1) in vec3 vert_col;     //Questo nuova variabile serve per il colore dei vetto

out vec4 vert_color_out;
out vec2 vert_pos_out;

void main()
{
    gl_Position = vec4(vert_pos, 1.f);  
    vert_color_out = vec4(vert_col,1.f);
    vert_pos_out = vec2(vert_pos.x, vert_pos.y);
}