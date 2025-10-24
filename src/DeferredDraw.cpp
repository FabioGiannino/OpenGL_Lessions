#include "DeferredDraw.h"
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

/*
Deferred Draw: Nuova tipologia di renderizzazione delle immagini. A differenza di quanto visto finora, cioè Forwarding Draw, dove ogni pixel della window viene elaborato,
    il Deferred Draw sfrutta due pipeline distinte per calcolare prima quali sono i punti da visualizzare e poi quale colorazione devono assumere in base alle luci che li colpiscono.
    Riduce di molto la pesantezza del calcolo in presenza di più luci e più mesh, perchè effettua il calcolo solo per i punti interessati, invece che includere anche i punti scartati dall'occlusione tipo.
    Il funzionamento è simile al PostFX, dove c'è la prima pipeline che renderizza l'oggetto SENZA LUCE, viene fatto uno "screenshot" e passato alla seconda pipeline, che calcola le luci
    solo per i punti "sopravvissuti" della prima pipeline
*/


DeferredDraw::DeferredDraw() 
{
    SceneProgram = new OpenGL_Program("resources/shaders/Deferred_Scene.vert", "resources/shaders/Deferred_Scene.frag");
    

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


    #pragma region GBUFFER
    //Preprariamo il framebuffer GBUFFER a disegnare "off screen"
    glGenFramebuffers(1, &GFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, GFbo);

    //Il GBuffer è un frame che si occupa di portare le informazioni di Colore(DIFFUSE), NORMALI E POSIZIONE degli oggetti in scena,
    // Poichè dovranno essere inviati anche a un'altra pipeline, verràno inviati sotto forma di TEXTURE

    //1. Render target Diffuse (8 bit per ogni colore, quindi 3 byte rgb)
    glGenTextures(1, &DiffuseTexture);
    glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);    //Creiamo lo spazio per una texture2d
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DiffuseTexture, 0);  //Attacchiamo la texture al buffer ATTACHMENT0


    //2. Render target Normal (3 byte x float -> il colore lo teniamo in float?)
    glGenTextures(1, &NormalTexture);
    glBindTexture(GL_TEXTURE_2D, NormalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);    //Creiamo lo spazio per una texture2d
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalTexture, 0);  //Attacchiamo la texture al buffer ATTACHMENT1

    //2. Render target Position (3 byte x float -> il colore lo teniamo in float?)
    glGenTextures(1, &PositionTexture);
    glBindTexture(GL_TEXTURE_2D, PositionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);    //Creiamo lo spazio per una texture2d
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, PositionTexture, 0);  //Attacchiamo la texture al buffer ATTACHMENT2

    GLenum Attachs[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, Attachs);


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
    #pragma endregion   //GBUFFER


    #pragma region Blending
    //Esattamente con per il Postfx, nel Deferred Draw la seconda pipeline prende lo "screenshot" uscito fuori dalla prima pipeline 
    // e ci calcola il fattore luce solo sui punti che davvero devono essere illuminati
    BlendingProgram = new OpenGL_Program("resources/shaders/Deferred_Blend.vert", "resources/shaders/Deferred_Blend.frag");
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
    glGenVertexArrays(1,&BlendingVao);
    glBindVertexArray(BlendingVao);

    glGenBuffers(1,&BlendingVbo);
    glBindBuffer(GL_ARRAY_BUFFER, BlendingVbo);
    size_t QuadDataSize = QuadVertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, QuadDataSize, QuadVertices.data(), GL_STATIC_DRAW);

    GLuint QuadLocation_0 = 0;
    glVertexAttribPointer(QuadLocation_0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(QuadLocation_0);

    GLuint QuadLocation_1 = 1;
    glVertexAttribPointer(QuadLocation_1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3* sizeof(float)));
    glEnableVertexAttribArray(QuadLocation_1);
    #pragma endregion //Blending

    //ILLUMINAZIONE DI PHONG -> Si calcola con il blending
    BlendingProgram->Bind();    
    glm::vec3 CameraPosition = glm::vec3(0.5f,0.5f,1.f);
    BlendingProgram->SetUniform("camera_position", CameraPosition);       
   
    //3 fonti di luce
    BlendingProgram->SetUniform("light_count", 3);      

    glm::vec3 PointLightRight = glm::vec3(4.0f, 0, 0.f);
    glm::vec3 PointLightUp = glm::vec3(0.0f, 4.0, 0.f);
    glm::vec3 PointLightLeft = glm::vec3(-4.0f, 0, 0.f);
    BlendingProgram->SetUniform("point_light_pos[0]", PointLightRight);          //Così si passano i dati degli array
    BlendingProgram->SetUniform("point_light_pos[1]", PointLightRight);    
    BlendingProgram->SetUniform("point_light_pos[2]", PointLightRight);    

}


DeferredDraw::~DeferredDraw() 
{
    glDeleteVertexArrays(1, &SceneVao);
    glDeleteBuffers(1, &Vbo);
    delete StormTextureId;
    delete SceneProgram;

    glDeleteTextures(1, &DiffuseTexture);
    glDeleteTextures(1, &NormalTexture);
    glDeleteTextures(1, &PositionTexture);
    glDeleteRenderbuffers(1, &SceneRbo);
    glDeleteFramebuffers(1, &GFbo);
}

//Funzione di debug-> mettiamo in modalità read il buffer della GFbo e in modalità write il buffer della swapchain
static void DebugGBuffer(GLuint GFbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);       //SwapChain framebuffer: Read + Write operation. Lo mettiamo anche in scrittura

    glClearColor(0.5f,0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, GFbo);      //GL_READ_FRAMEBUFFER = read operation
    
    /*
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(                             //FUNZIONE BLIT -> Sposta i dati da un framebuffer readable in un framebuffer writable
        0, 0, 800, 600,                             //Bounds del buffer di origine
        0, 300, 400, 600,                               //Bounds buffer di destinazione
        GL_COLOR_BUFFER_BIT,                        //Indica il buffer di destinazione
        GL_LINEAR                                   //interpolazione
    );
    */

    //Renderizziamo le tre informazioni: Diffuse - World Normal - World Position
    glReadBuffer(GL_COLOR_ATTACHMENT0);             //Diffuse
    glBlitFramebuffer(                             
        0, 0, 800, 600,                            
        0, 300, 400, 600,                          
        GL_COLOR_BUFFER_BIT,                       
        GL_LINEAR                                  
    );

    glReadBuffer(GL_COLOR_ATTACHMENT1);             //World Normal  ->RGB diventano i colori delle direzioni
    glBlitFramebuffer(                              //Red -> right, le normali che pt a destra
        0, 0, 800, 600,                             //Green -> up, le normali che puntano in alto
        400, 300, 800, 600,                         //Blue -> forward, le nomali che puntano fuori (la z va dentro, verso lo spettatore)
        GL_COLOR_BUFFER_BIT,                       
        GL_LINEAR                                  
    );

    
    glReadBuffer(GL_COLOR_ATTACHMENT2);             //World Position ->RGB diventano i colori delle posizioni
    glBlitFramebuffer(                              //Red -> right, i punti più a destra si colorano di rosso
        0, 0, 800, 600,                             //Green -> up, i punti più in alto si colorano di verde
        0, 0, 400, 300,                             //Blue -> forward, i punti più vicini allo schermo di colorano di blue
        GL_COLOR_BUFFER_BIT,                       
        GL_LINEAR                                  
    );
}


void DeferredDraw::Update(float InDeltaTime)
{
    static float ElapsedTime = 0;
    ElapsedTime += InDeltaTime;

    //1. Data Preparation Pass o Geometry Pass
    glBindFramebuffer(GL_FRAMEBUFFER, GFbo);        //bindiamo il nuovo frame buffer creato
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Operazione necessaria nell'update, in quando dopo si binda il programma di postFX
    glBindVertexArray(SceneVao);
    SceneProgram->Bind();
    StormTextureId->Bind(GL_TEXTURE0);


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



   
    //DebugGBuffer(GFbo);
   





    //2. Blending Pass
    //Si ritorna al frame buffer iniziale, quello Swapchain, cioè quello con front e back buffer usato per mostrare le immagini su monitor. 
    //Finora abbiamo lavorato su un buffer custom. ora bisogna collegarlo a quello da display
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   
    glDisable(GL_DEPTH_TEST);       //questo buffer rappresenterà solo una texture, non gli servono nè depth ne cull face
    glDisable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(BlendingVao);
    BlendingProgram->Bind();

    //Il nuovo programma bindato richiede 3 texture -> la diffuse, la normal e la position
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, DiffuseTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, NormalTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, PositionTexture);

    glDrawArrays(GL_TRIANGLES, 0 , 6); //disegno i due triangoli per la texture



}