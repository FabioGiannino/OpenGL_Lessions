#pragma once
#include <glad/glad.h>
//#include "OpenGL_Program.h"

class TexurePerspective
{
public:
    TexurePerspective();
    ~TexurePerspective();
    void Update(float InDeltaTime);
private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    class OpenGL_Texture* SmileTextureID;       //come da spiegazione sotto
    class OpenGL_Texture* BoxTextureID;
    //Forward declaration: dichiariamo il simbolo qua dentro invece di importarci il .h, così da diminuire le dipendenze tra header.
    // Sarà poi il linker a sistemare tutto prendendo le corrette dichiarazioni dal file.cpp 
    class OpenGL_Program* Program;  
        //Nota -> Ridurre le dipendenze tra headers aiuta anche la compilazione a essere più pulita: Se avessimo importato OpenGL_Program.h qui
        // e quel file header non venisse compilato, ANCHE tutti i file CPP che importano questo header NON SAREBBERO compilati 
};
