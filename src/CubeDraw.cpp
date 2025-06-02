#include "CubeDraw.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "OpenGL_Program.h"
#include <cmath>
#include "OpenGL_Texture.h"

CubeDraw::CubeDraw() 
{
    Program = new OpenGL_Program("resources/shaders/Cube.vert", "resources/shaders/Cube.frag");
    
    std::vector<float> Vertices = {
        //Position     //Uvs
        //FRONT FACE
        -1, -1,  1,     0, 0,  // bottom-left
         1, -1,  1,     1, 0,  // bottom-right
         1,  1,  1,     1, 1,  // top-right  
        -1,  1,  1,     0, 1,  // top-left
        -1, -1,  1,     0, 0,  // bottom-left
         1,  1,  1,     1, 1,  // top-right  
        
        // BACK FACE
         1, -1, -1,     0, 0,  // bottom-left
        -1, -1, -1,     1, 0,  // bottom-right
        -1,  1, -1,     1, 1,  // top-right  
         1,  1, -1,     0, 1,  // top-left
         1, -1, -1,     0, 0,  // bottom-left
        -1,  1, -1,     1, 1,  // top-right  
 
        //LEFT FACE
        -1, -1,-1,      0, 0,    //bottom-left
        -1, -1, 1,      1, 0,    //bottom-right
        -1,  1, 1,      1, 1,    //top-right
        -1,  1,-1,      0, 1,    //top-left 
        -1, -1,-1,      0, 0,    //bottom-left
        -1,  1, 1,      1, 1,    //top-right

        //RIGHT FACE      
         1, -1, 1,      0, 0,    //bottom-left
         1, -1,-1,      1, 0,    //bottom-right
         1,  1,-1,      1, 1,    //top-right
         1,  1, 1,      0, 1,    //top-left 
         1, -1, 1,      0, 0,    //bottom-left
         1,  1,-1,      1, 1,    //top-right

         //TOP FACE      
         -1, 1, 1,      0, 0,    //bottom-left
          1, 1, 1,      1, 0,    //bottom-right
          1, 1,-1,      1, 1,    //top-right
         -1, 1,-1,      0, 1,    //top-left 
         -1, 1, 1,      0, 0,    //bottom-left
          1, 1,-1,      1, 1,    //top-right
         
         //BOTTOM FACE
         -1,-1,-1,      0, 0,    //bottom-left
          1,-1,-1,      1, 0,    //bottom-right
          1,-1, 1,      1, 1,    //top-right
         -1,-1, 1,      0, 1,    //top-left 
         -1,-1,-1,      0, 0,    //bottom-left
          1,-1, 1,      1, 1,    //top-right
    };

    //1. Create VAO
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);

    //2. Crate VBO to load data
    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    size_t DataSize = Vertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW);

    //3. Link to Vertex Shader
    GLuint Location_0 = 0;
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE,  5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 2, GL_FLOAT, GL_FALSE,  5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(Location_1);

    //5. Set Viewport
    glViewport(0, 0, 800, 600);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    Program->Bind();

    //6. Create Texture
    BoxTextureId = new OpenGL_Texture("resources/textures/wood-box.jpg");
    BoxTextureId->Bind(GL_TEXTURE0);

    //7. Abilitiamo il depth test, cioè lo z buffer, il calcolo per capire se un vertice viene mostrato o meno a seconda della sua z
    glEnable(GL_DEPTH_TEST);

    //8. Abilitiamo il face culling, cioè il calcolo per capire se una faccia di una figura 3d viene mostrata o nascosta alla camera
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK) //default
    //glFrontFace(GL_CCW) //default
}

CubeDraw::~CubeDraw() 
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    delete BoxTextureId;
    delete Program;
}

void CubeDraw::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float ElapsedTime = 0;
    ElapsedTime += InDeltaTime;

    Program->SetUniform("angle", 20.f * ElapsedTime);       //lo facciamo ruotare

    glDrawArrays(GL_TRIANGLES, 0, 36);      //disegnamo 6 vertici per ogni faccia, ci sono 6 facce -> 36 vertici in triangoli
}