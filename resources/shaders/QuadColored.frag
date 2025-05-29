#version 460 core       

out vec4 frag_color;    

//dobbiamo colorare il quadrato attraverso un interpolazione dei colori dei suoi vertici
in vec4 vert_color_out; //dichiariamo la variabile del colore in entrata

void main()
{
    //frag_color = vec4(1.f, 0.f, 0.f, 1.f);
    frag_color = vert_color_out;
}