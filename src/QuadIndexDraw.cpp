//In questa nuova versione del QuadDraw, invece di disegnare due triangoli distinti partendo da sei vertici, si disegneranno quattro vertici e li si indicizzano
#include "QuadIndexDraw.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "OpenGL_Program.h"


//In questo caso, disegneremo il quadrato usando solo quattro vertici, a differenza dei sei per i due triangoli
// Sarà necessario usare un altro buffer ad-hoc per i vertici dei triangoli
QuadIndexDraw::QuadIndexDraw()
{
    Program = new OpenGL_Program("resources/shaders/triangle.vert","resources/shaders/triangle.frag");

    std::vector<float> Vertices = {
        -0.5f,  - 0.5f, 0.0f,       //basso sx
        0.5,    -0.5f,  0.0f,       //basso dx   
        0.5,    0.5f,  0.0f,        //alto dx       
        -0.5f,   0.5f,   0.0f       //alto sx
    };

    std::vector<uint32_t> Indexes = {
        0,  1,  2,          //Triangolo di destra
        0,  2,  3           //triangolo di sinistra
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


    //3.1- Creazione del EBO ->ELEMENT BUFFER OBJECT, un nuovo buffer di supporto per operazioni più complesse
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indexes.size() * sizeof(uint32_t), Indexes.data(), GL_STATIC_DRAW);

    //4- Per ultimo, possiamo modificare la viewport dove disegnare l'immagine. Di default prenderebbe tutta la finestra
    glViewport(0, 0, 800, 600);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);  
    Program->Bind();                
}

QuadIndexDraw::~QuadIndexDraw()
{   
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    delete Program;
}

void QuadIndexDraw::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);   //Ad ogni update puliamo il buffer 
    //glDrawArrays(GL_TRIANGLES, 0, 6);   
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);   //!Usando DrawElements invece che DrawArray, la GPU Andrà a leggere all'interno dell'EBO creata
            //^^ La EBO poi dirà alla GPU di mostrare gli elementi dentro la VAO secondo gli indici specificati
}