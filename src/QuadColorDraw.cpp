#include "QuadColorDraw.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "OpenGL_Program.h"


//Disegnare il quadrato implica disegnare due triangoli rettangoli isosceli congruenti, colorandolo
QuadColorDraw::QuadColorDraw()
{
    Program = new OpenGL_Program("resources/shaders/QuadColored.vert","resources/shaders/QuadColored.frag");

    std::vector<float> Vertices = {
        //Triangolo di sinistra         //Color RGB
        -0.5f,  -0.5f, 0.0f,            1.f, 0.f, 0.f,
        0.5,    -0.5f,  0.0f,           0.f, 1.f, 0.f,
        -0.5f,   0.5f,   0.0f,          0.f, 0.f, 1.f,

        //Triangolo di destra
        -0.5f,   0.5f,   0.0f,          0.f, 0.f, 1.f, 
        0.5,    -0.5f,  0.0f,           0.f, 1.f, 0.f,
        0.5,    0.5f,  0.0f,            1.f, 1.f, 0.f
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
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 6* sizeof(float), (void*)0);   //MODIFICHIAMO A 6 L'OFFSET. ->Dal pt 0 legge 3 elementi e poi ogni 6 elementi legge altri tre elementi
    glEnableVertexAttribArray(Location_0);      

    //Questo nuovo buffer invece è quello dei COLORI, che è composto anche lui da 3 elementi (RGB) ma viene letto dall'offset 3 e di 6 in 6
    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 3, GL_FLOAT, GL_FALSE, 6* sizeof(float), (void*)(3*sizeof(float)));   
    glEnableVertexAttribArray(Location_1);      


    //4- Per ultimo, possiamo modificare la viewport dove disegnare l'immagine. Di default prenderebbe tutta la finestra
    glViewport(0, 0, 800, 600);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);  
    Program->Bind();                  

}

QuadColorDraw::~QuadColorDraw()
{   
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    delete Program;
}

void QuadColorDraw::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);   //Ad ogni update puliamo il buffer 
    glDrawArrays(GL_TRIANGLES, 0, 6);   //disegnamo i 6 vertici, in modalità "Triangolo"
}