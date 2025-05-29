#version 460 core      

layout (location = 0) in vec3 vert_pos; 
layout (location = 1) in vec3 vert_col;     //Questo nuova variabile serve per il colore dei vetto

out vec4 vert_color_out;

//Scale dell'immagine -> moltiplichiamo la posizione dei vertici per un float
uniform float scale;

void main()
{
    vec3 vert_pos_scaled = vert_pos * scale;
    gl_Position = vec4(vert_pos_scaled, 1.f);  
    vert_color_out = vec4(vert_col,1.f);
}