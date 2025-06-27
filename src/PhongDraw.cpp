#include "PhongDraw.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "OpenGL_Program.h"
#include <cmath>
#include "OpenGL_Texture.h"
#include "ObjParser.h"
#include <glm/glm.hpp>      //libreria usata per il calcolo matriciale
#include <glm/ext.hpp>

PhongDraw::PhongDraw() 
{
    Program = new OpenGL_Program("resources/shaders/Phong.vert", "resources/shaders/Phong.frag");
    

    //A differenza degli altri programmi, dove dichiavamo noi stessi i valori dei vertici,
    //qui dobbiamo tirarli fuori dal file Obj. Ci viene in aiuto quindi l'objParser fatto tempo addietro
    Obj obj;
    ObjParser::TryParseObj("resources/models/stormtrooper.obj",obj);

    
    for (size_t i  = 0; i< obj.triangles.size(); ++i)
    {
        auto& t = obj.triangles[i];

        //Vertice 1
        Vertices.push_back(t.v1.point.x);   //posizione
        Vertices.push_back(t.v1.point.y);
        Vertices.push_back(t.v1.point.z);
        Vertices.push_back(t.v1.uv.x);      //UV
        Vertices.push_back(t.v1.uv.y);
        Vertices.push_back(t.v1.normal.x);  //normali
        Vertices.push_back(t.v1.normal.y);
        Vertices.push_back(t.v1.normal.z);

        //Vertice 2
        Vertices.push_back(t.v2.point.x);   //posizione 
        Vertices.push_back(t.v2.point.y);
        Vertices.push_back(t.v2.point.z);
        Vertices.push_back(t.v2.uv.x);      //UV
        Vertices.push_back(t.v2.uv.y);
        Vertices.push_back(t.v2.normal.x);  //normali
        Vertices.push_back(t.v2.normal.y);
        Vertices.push_back(t.v2.normal.z);

        //Vertice 3
        Vertices.push_back(t.v3.point.x);   //posizione
        Vertices.push_back(t.v3.point.y);
        Vertices.push_back(t.v3.point.z);        
        Vertices.push_back(t.v3.uv.x);      //UV
        Vertices.push_back(t.v3.uv.y);        
        Vertices.push_back(t.v3.normal.x);  //normali
        Vertices.push_back(t.v3.normal.y);
        Vertices.push_back(t.v3.normal.z);
    }


    //1. Create VAO
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);

    //2. Crate VBO to load data
    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    size_t DataSize = Vertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW);

    //3. Link to Vertex Shader
    GLuint Location_0 = 0;          //questi caricano i vertici (3 dimensioni)
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE,  8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    GLuint Location_1 = 1;          //questi caricano le uv (2 dimensioni)
    glVertexAttribPointer(Location_1, 2, GL_FLOAT, GL_FALSE,  8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(Location_1);

    GLuint Location_2 = 2;          //questi caricano le normali (3 dimensioni) 
    glVertexAttribPointer(Location_2, 3, GL_FLOAT, GL_FALSE,  8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(Location_2);


    //5. Set Viewport
    glViewport(0, 0, 800, 600);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    Program->Bind();

    //6. Create Texture
    StormTextureId = new OpenGL_Texture("resources/models/stormtrooper.png");
    StormTextureId->Bind(GL_TEXTURE0);

    //7. Abilitiamo il depth test, cioè lo z buffer, il calcolo per capire se un vertice viene mostrato o meno a seconda della sua z
    glEnable(GL_DEPTH_TEST);

    //8. Abilitiamo il face culling, cioè il calcolo per capire se una faccia di una figura 3d viene mostrata o nascosta alla camera
    glEnable(GL_CULL_FACE);


    //Ora iniziamo a mettere le varie trasformazioni della scena. 
    //Partiamo dalla camera
    glm::vec3 Position = glm::vec3(0, 0, 8);        //nella z = 8
    glm::vec3 Direction = glm::vec3(0, 0, -1);      //guarda verso l'asse z negativa
    glm::vec3 Up = glm::vec3(0, 1, 0);              //l'up è l'asse y, quindi non è affetta da nessuna rotazione 
    //altri parametri della camera e della scena
    float FovY = 60.0f;         
    float AspectRatio = 800.f / 600.f;
    float ZNear = 0.01f;
    float ZFar = 100.0f;

    //Creazione di una matrice di View della camera. Si usa LookAt e gli si passano come info: la posizione, il dove sta guardando e la rotazione
    View = glm::lookAt(Position, Position + Direction, Up);
    //Creazione della matrice prospettica, usando perspective. gli si passa il fov, l'aspect ratio, il near e il far e se la crea in automatico
    Projection = glm::perspective(glm::radians(FovY), AspectRatio, ZNear, ZFar);


    //ILLUMINAZIONE DI PHONG -> carichiamo nel fragm shader una point light, passandogli la pos di quest'ultima
    glm::vec3 PointLightPos = glm::vec3(5.0f, 0, 5.0f);
    Program->SetUniform("point_light_pos", PointLightPos);

    //Specular -> Passiamo la posizione della camera 
    Program->SetUniform("camera_position", Position);
}

PhongDraw::~PhongDraw() 
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    delete StormTextureId;
    delete Program;
}

void PhongDraw::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float ElapsedTime = 0;
    ElapsedTime += InDeltaTime;

    //rotazione dello stormtrooper
    float Angle = 20.0f * ElapsedTime;
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::translate(Model, glm::vec3(0,-4,0));
    Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(0, 1, 0));
    Model = glm::scale(Model, glm::vec3(2.f));

    //Creazione della Model-View-Projection Matrix, la matrice che converte le coordinate nei vari spazi
    glm::mat4 Mvp = Projection * View * Model;        //La sequenza di moltiplicazioni sarà Model poi View poi Projection, quindi l'ordine inverso indica la seq di moltiplicazioni
    
    Program->SetUniform("mvp", Mvp);       //carichiamo la matrice mvp, con tutte le trasformazioni, nello shader

    Program->SetUniform("model", Model);    //carichiamo la model singolare perchè per l'illum. di Phong abbiamo bisogno delle coordinate World Space (e ora c'è solo la local o direttamente lo screen space)


    glDrawArrays(GL_TRIANGLES, 0, Vertices.size());   //disegnamo tutti i vertici dello stormtrooper
}