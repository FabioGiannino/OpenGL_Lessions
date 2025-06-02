#include "TexurePerspective.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "OpenGL_Program.h"
#include "OpenGL_Texture.h"

/*Tutto il calcolo del fov e dell'aspect ratio viene fatto all'interno del vertex shader
    Il risultato è quello di una texture perfettamente quadrata
*/
TexurePerspective::TexurePerspective()
{
    Program = new OpenGL_Program("resources/shaders/QuadPerspective.vert","resources/shaders/QuadPerspective.frag");

    std::vector<float> Vertices = {
        //Triangolo di destra         //UVs DELLA TEXTURE 
        -0.5f,  -0.5f, 0.0f,            0.f, 0.f, 
        0.5,    -0.5f,  0.0f,           1.f, 0.f, 
        0.5f,   0.5f,   0.0f,          1.f, 1.f, 

        //Triangolo di destra
        -0.5f,  -0.5f,   0.0f,          0.f, 0.f, 
        0.5,     0.5f,  0.0f,           1.f, 1.f,
        -0.5,    0.5f,  0.0f,           0.f, 1.f
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
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)0);   //MODIFICHIAMO A 5 L'OFFSET. ->Dal pt 0 legge 3 elementi e poi ogni 5 elementi legge altri tre elementi
    glEnableVertexAttribArray(Location_0);      

    //Questo buffer lo dedichiamo alle UV map della texture in entrata, che sono dei vector due 
    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3*sizeof(float)));   
    glEnableVertexAttribArray(Location_1);      


    //4- Per ultimo, possiamo modificare la viewport dove disegnare l'immagine. Di default prenderebbe tutta la finestra
    glViewport(0, 0, 800, 600);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);  
    Program->Bind();                  

    //5 - Creiamo la texture 
    SmileTextureID = new OpenGL_Texture("resources/textures/smile.png");
    BoxTextureID = new OpenGL_Texture("resources/textures/wood-box.jpg");
    
    //Smile
    SmileTextureID->Bind(GL_TEXTURE0);
    //Wood Box
    BoxTextureID->Bind(GL_TEXTURE1);

    //6 - Attivare l'ALPHA BLENDING
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      //Attiviamo il blending dei colori per impostare la gestione della trasparenza


}

TexurePerspective::~TexurePerspective()
{   
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);    
    delete SmileTextureID;
    delete BoxTextureID;
    delete Program;
}

void TexurePerspective::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);   //Ad ogni update puliamo il buffer 
    glDrawArrays(GL_TRIANGLES, 0, 6);   //disegnamo i 6 vertici, in modalità "Triangolo"
}