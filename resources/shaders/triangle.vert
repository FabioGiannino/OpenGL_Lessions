#version 460 core       //qui si mette la versione di OpenGL e la modalità (core)

//Questo è il vertex shader del triangolo. Le istruzioni qui dentro vengono eseguite
//PER OGNI VERTICE

layout (location = 0) in vec3 vert_pos; //Dichiaro la variabile "vert_pos" che è un vector3 passato in input alla posizione 0 dei dati

void main()
{
    gl_Position = vec4(vert_pos, 1.f);  //L'output sarà una modifica alla posizione, che restituisce un vector4, ma non è stato spiegato cos'è l'ultimo elemento

}