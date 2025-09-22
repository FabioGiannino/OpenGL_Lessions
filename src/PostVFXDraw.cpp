#include "PostVFXDraw.h"
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

PostVFXDraw::PostVFXDraw() 
{
    SceneProgram = new OpenGL_Program("resources/shaders/PostFX_scene.vert", "resources/shaders/PostFX_scene.frag");
    

    //A differenza degli altri programmi, dove dichiavamo noi stessi i valori dei vertici,
    //qui dobbiamo tirarli fuori dal file Obj. Ci viene in aiuto quindi l'objParser fatto tempo addietro
    Obj obj;
    ObjParser::TryParseObj("resources/models/stormtrooper.obj",obj);

    
    for (size_t i  = 0; i< obj.triangles.size(); ++i)
    {
        auto& t = obj.triangles[i];

        //Vertice 1
        SceneVertices.push_back(t.v1.point.x);   //posizione
        SceneVertices.push_back(t.v1.point.y);
        SceneVertices.push_back(t.v1.point.z);
        SceneVertices.push_back(t.v1.uv.x);      //UV
        SceneVertices.push_back(t.v1.uv.y);
        SceneVertices.push_back(t.v1.normal.x);  //normali
        SceneVertices.push_back(t.v1.normal.y);
        SceneVertices.push_back(t.v1.normal.z);

        //Vertice 2
        SceneVertices.push_back(t.v2.point.x);   //posizione 
        SceneVertices.push_back(t.v2.point.y);
        SceneVertices.push_back(t.v2.point.z);
        SceneVertices.push_back(t.v2.uv.x);      //UV
        SceneVertices.push_back(t.v2.uv.y);
        SceneVertices.push_back(t.v2.normal.x);  //normali
        SceneVertices.push_back(t.v2.normal.y);
        SceneVertices.push_back(t.v2.normal.z);

        //Vertice 3
        SceneVertices.push_back(t.v3.point.x);   //posizione
        SceneVertices.push_back(t.v3.point.y);
        SceneVertices.push_back(t.v3.point.z);        
        SceneVertices.push_back(t.v3.uv.x);      //UV
        SceneVertices.push_back(t.v3.uv.y);        
        SceneVertices.push_back(t.v3.normal.x);  //normali
        SceneVertices.push_back(t.v3.normal.y);
        SceneVertices.push_back(t.v3.normal.z);
    }


    //1. Create VAO
    glGenVertexArrays(1, &SceneVao);
    glBindVertexArray(SceneVao);

    //2. Crate VBO to load data
    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    size_t DataSize = SceneVertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, SceneVertices.data(), GL_STATIC_DRAW);

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
    

    //6. Create Texture
    StormTextureId = new OpenGL_Texture("resources/models/stormtrooper.png");
    //StormTextureId->Bind(GL_TEXTURE0);  ->Spostato nell'update 

    //7. Abilitiamo il depth test, cioè lo z buffer, il calcolo per capire se un vertice viene mostrato o meno a seconda della sua z
    //glEnable(GL_DEPTH_TEST);

    //8. Abilitiamo il face culling, cioè il calcolo per capire se una faccia di una figura 3d viene mostrata o nascosta alla camera
    //glEnable(GL_CULL_FACE);


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
    glm::vec3 PointLightPos = glm::vec3(4.0f, 0, 0);
    SceneProgram->Bind();        //Anche se viene già bindato nell'update, è necessario farlo anche prima del set uniform.
    SceneProgram->SetUniform("point_light_pos", PointLightPos);

    //POST FX: Preprariamo un framebuffer a disegnare "off screen", cioè un nuovo frame buffer personalizzato
    //  dove inseriamo nuovi shader prima di essere inviati a video.
    // -> Quando sul frag shader scriviamo una nuova var "out", questa var è parte dell'output del Back Buffer.
    //      Noi collegheremo gli output del back buffer agli "in" del nuovo buffer, dove inseriamo i PostFx
    glGenFramebuffers(1, &SceneFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, SceneFbo);

    //Ora definiamo che risorse ha questo frame buffer. Noi vogliamo aggiungere un filtro di colorazione, quindi il colore è uno
    //1- Attach Color ->  passiamo il colore dal back buffer allo scene buffer sotto forma di TEXTURE (si può fare sotto forma di texture o di buffer, ma il secondo non conviene perchè è più per lo storage)
    glGenTextures(1, &SceneTexture);
    glBindTexture(GL_TEXTURE_2D, SceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);    //Creiamo lo spazio per una texture2d
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SceneTexture, 0);  //Attacchiamo la texture al buffer

    //2- Attach Depth to FrameBuffer -> Nella funz update, la prima cosa si fa è pulire il color_buffer e il depth buffer. Se inseriamo un
    //      buffer di mezzo tra il back e il front per i postVFX, quello che succede è che perdiamo la depth! Dobbiamo aggiungerla
    glGenRenderbuffers(1, &SceneRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, SceneRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 800, 600);     //Creiamo il render buffer della stessa dim del texture buffer e 24bit di profondità
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, SceneRbo);
    glBindRenderbuffer(GL_RENDERBUFFER,0);       //Unbind render buffer: lo scolleghiamo perchè c'è il rischio che con le successive operazioni (quelle dell'update), venga utilizzato questo e non l'altro
                                                //Solitamente è buona pratica che ogni risorsa bindata venga sbindata, anche se non sempre in quanto è un operazione onerosa e quindi non performante da fare in update
    
    bool RboSuccess = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if(!RboSuccess)
    {
        std::cout << "Error creating framebuffer\n";
        std::runtime_error("Error creating framebuffer!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);            //reimpostiamo come inizio lo swapchain di default (possibile solo per il frame buffer, se no bisogna specificare il nuovo bind)



    //La pipeline sopra è quella che alla fine crea la scena complessiva. la seconda pipeline fa uno screenshot della scena e la tratta come una texture
    // su cui fare dei rimaneggiamenti, i post fx
    FXProgram = new OpenGL_Program("resources/shaders/PostFX_effect.vert", "resources/shaders/PostFX_effect.frag");
    std::vector<float> QuadVertices = {         //Sono già in NDC, quindi i valori saranno tra -1 e 1
         //Triangle Right    //Uvs
        -1.f, -1.f, 0.f,   0.f, 0.f,  // bottom-left
         1.f, -1.f, 0.f,   1.f, 0.f,  // bottom-right
         1.f,  1.f, 0.f,   1.f, 1.f,  // top-right  

        //Triangle Left
        -1.f, -1.f, 0.f,   0.f, 0.f,  // bottom-left
         1.f,  1.f, 0.f,   1.f, 1.f,  // top-right    
        -1.f,  1.f, 0.f,   0.f, 1.f   // top-left
    };
    //1- crea il VAO
    glGenVertexArrays(1,&FxVao);
    glBindVertexArray(FxVao);

    glGenBuffers(1,&FxVbo);
    glBindBuffer(GL_ARRAY_BUFFER, FxVbo);
    size_t QuadDataSize = QuadVertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, QuadDataSize, QuadVertices.data(), GL_STATIC_DRAW);

    GLuint QuadLocation_0 = 0;
    glVertexAttribPointer(QuadLocation_0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(QuadLocation_0);

    GLuint QuadLocation_1 = 1;
    glVertexAttribPointer(QuadLocation_1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3* sizeof(float)));
    glEnableVertexAttribArray(QuadLocation_1);
}


PostVFXDraw::~PostVFXDraw() 
{
    glDeleteVertexArrays(1, &SceneVao);
    glDeleteBuffers(1, &Vbo);
    delete StormTextureId;
    delete SceneProgram;
}



void PostVFXDraw::Update(float InDeltaTime)
{
    //1. Draw Scene (Pass 1)
    glBindFramebuffer(GL_FRAMEBUFFER, SceneFbo);        //bindiamo il nuovo frame buffer creato
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Operazione necessaria nell'update, in quando dopo si binda il programma di postFX
    glBindVertexArray(SceneVao);
    SceneProgram->Bind();
    StormTextureId->Bind(GL_TEXTURE0);

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
    
    SceneProgram->SetUniform("mvp", Mvp);       //carichiamo la matrice mvp, con tutte le trasformazioni, nello shader

    SceneProgram->SetUniform("model", Model);    //carichiamo la model singolare perchè per l'illum. di Phong abbiamo bisogno delle coordinate World Space (e ora c'è solo la local o direttamente lo screen space)


    glDrawArrays(GL_TRIANGLES, 0, SceneVertices.size());   //disegnamo tutti i vertici dello stormtrooper

    //2. Post FX (Pass 2)
    //Si ritorna al frame buffer iniziale, quello Swapchain, cioè quello con front e back buffer usato per mostrare le immagini su monitor. 
    //Finora abbiamo lavorato su un buffer custom. ora bisogna collegarlo a quello da display
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   
    glDisable(GL_DEPTH_TEST);       //questo buffer rappresenterà solo una texture, non gli servono nè depth ne cull face
    glDisable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(FxVao);
    FXProgram->Bind();

    //Il nuovo programma bindato richiede una texture (nel fragm shader PostFX_Effect.frag)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, SceneTexture);
    glDrawArrays(GL_TRIANGLES, 0 , 6); //disegno i due triangoli per la texture



}