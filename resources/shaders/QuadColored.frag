#version 460 core       

out vec4 frag_color;    

//dobbiamo colorare il quadrato attraverso un interpolazione dei colori dei suoi vertici
in vec4 vert_color_out; //dichiariamo la variabile del colore in entrata

//Definiamo una varabile che rimane "costante" durante le drawcall -> i vertici possono cambiare, durante una draw call, mentre una uniform var no
uniform vec4 base_color;

void main()
{
    frag_color = base_color * vert_color_out;

}