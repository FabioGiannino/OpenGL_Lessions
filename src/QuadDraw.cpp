#include "QuadDraw.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "OpenGL_Program.h"

/*Una cosa che si nota di più nella rasterizzazione del quadrato rispetto a quella del triangolo è la mancanza di un fix per l'aspect Ratio
Difatti si nota come il risultato finale è più un rettangolo, a causa del fatto che si espande in base alla grandezza della viewport*/


//Disegnare il quadrato implica disegnare due triangoli rettangoli isosceli congruenti
QuadDraw::QuadDraw()
{
    Program = new OpenGL_Program("resources/shaders/triangle.vert","resources/shaders/triangle.frag");

    std::vector<float> Vertices = {
        //Triangolo di sinistra
        -0.5f,  - 0.5f, 0.0f,           
        0.5,    -0.5f,  0.0f,           
        -0.5f,   0.5f,   0.0f,

        //Triangolo di destra
        -0.5f,   0.5f,   0.0f,
        0.5,    -0.5f,  0.0f,   
        0.5,    0.5f,  0.0f        
    };

    //1- Ora si crea il VAO ->Vertex Array Object: è essenzialmente un contenitore di Buffer e serve per attivare o disattivare più buffer contemporamente
    glGenVertexArrays(1,&VAO);    
    glBindVertexArray(VAO);         

    //2- Ora si crea il VBO ->Vertex Buffer Object: è il buffer da infilare dentro il VAO
    glGenBuffers(1,&VBO);       
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Resiziamo il buffer alla grandezza in byte del vettore di vertici -> 9*floatDimension
    size_t DataSize = Vertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW);

    //3- Ora dobbiamo BINDARE i dati del buffer allo stage del VERTEX SHADER, perchè se no la GPU non sa che farsene
    GLuint Location_0 = 0;
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);   
    glEnableVertexAttribArray(Location_0);      

    //4- Per ultimo, possiamo modificare la viewport dove disegnare l'immagine. Di default prenderebbe tutta la finestra
    glViewport(0, 0, 800, 600);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);  
    Program->Bind();                  

    //Modalità di disegno, mostrando SOLO i punti dei vertici
    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    //glPointSize(8);

    //Altra modalità di disegno, mostrando SOLO le linee
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glLineWidth(20);


}

QuadDraw::~QuadDraw()
{   
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    delete Program;
}

void QuadDraw::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);   //Ad ogni update puliamo il buffer 
    glDrawArrays(GL_TRIANGLES, 0, 6);   //disegnamo i 6 vertici, in modalità "Triangolo"
}