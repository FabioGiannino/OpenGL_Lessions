#version 460 core       //qui si mette la versione di OpenGL e la modalità (core)

//Questo è il vertex shader del triangolo. Le istruzioni qui dentro vengono eseguite
//PER OGNI PIXEL RASTERIZZATO

out vec4 frag_color;    //dichiaro una variabile di output di tipo vector4
void main()
{
    frag_color = vec4(1.f, 0.f, 0.f, 1.f);
}