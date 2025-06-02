#include "TextureDraw.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "OpenGL_Program.h"

//essendp STB_IMAGE una single header implementation, dobbiamo defiire questo simbolo per usarlo
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Color
{
    float R;
    float G;
    float B;
    float A;
};

GLuint CreateTexture(const std::string& InFilePath)
{
    stbi_set_flip_vertically_on_load(true);     //per flippare sulla y la texture che si sta per creare (le UV sono invertite rispetto ai vertici delle mesh)

    int Width, Height, Channels;
    unsigned char* Data = stbi_load(InFilePath.c_str(), &Width, &Height, &Channels, 0);            //con questa API si carica l'immagine, e lui ritorna (sotto forma di puntatori input), l'altezza,larghezza e i canali dei coloi
    if (Data == NULL)           //Usciamo se l'immagine non è stata caricata correttamente
    {
        std::cout <<"Error Reading Image: "<< InFilePath;
        throw std::runtime_error("Error reading image");
    }

    GLenum Format = Channels ==  3 ? GL_RGB : GL_RGBA;      //se l'immagine ha 3 canali è un RGB, se ne ha 4, un RGBA

    GLuint TextureID;
    glGenTextures(1, &TextureID);       //Creiamo la texture e la bindiamo
    glBindTexture(GL_TEXTURE_2D,TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);   //carichiamo i dati della texture sulla gpu

    //Setting del Wrapping  -> si possono specificare wrapping diversi per l'asse x e y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);      //S è l'asse X, specifica il comportamento del wrapping lungo l'asse x. in questo caso è il REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);      //T è l'asse Y, specifica il comportamento del wrapping lungo l'asse y. in questo caso è il REPEAT
    
    //Setting del Filtering ->  Si possono specificare filering diversi se l'immagine si riduce o si ingrandisce
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //Se si riduce, fai un filtering lineare (con blend dei colori), sfruttando il mipmapping se possibile
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);          //Se si ingrandisce, fai un filtering nearest (Senza blend dei colori)

    //Attivazione del mipmap
    glGenerateMipmap(GL_TEXTURE_2D);

    return TextureID;
}


TextureDraw::TextureDraw()
{
    Program = new OpenGL_Program("resources/shaders/QuadTexture.vert","resources/shaders/QuadTexture.frag");

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
    SmileTextureID = CreateTexture("resources/textures/smile.png");
    glActiveTexture(GL_TEXTURE0);               //attiviamo la prima texture 
    glBindTexture(GL_TEXTURE_2D, SmileTextureID);   //è una ripetizione perchè teoricamente è già stata effettuata questa operazione quando si crea la texture. é però utile nel caso si creino più texture assieme

    //glUniform1i(glGetUniformLocation(Program->GetProgramID(),"smile_tex"),0);   //settiamo la var smile_tex a 0 del .frag
    // Abbiamo inserito questa logica direttamente nel fragment shader, con 'layout (binding = 0) uniform sampler2D smile_tex;'
}

TextureDraw::~TextureDraw()
{   
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);    
    glDeleteTextures(1, &SmileTextureID);
    delete Program;
}

void TextureDraw::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);   //Ad ogni update puliamo il buffer 
    glDrawArrays(GL_TRIANGLES, 0, 6);   //disegnamo i 6 vertici, in modalità "Triangolo"
}