//Classe contenente le funzionalità per creare e bindare una texture in opengl
#include "OpenGL_Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Forwarding declaration
static GLuint CreateTexture(const std::string& InFilePath);
static GLuint CreateRandomTexture(const int Width, const int Height, const int Channels);


OpenGL_Texture::OpenGL_Texture(const std::string& InImagePath)
{
    TexureId = CreateTexture(InImagePath);
}

OpenGL_Texture::OpenGL_Texture(const int InWidth, const int InHeight, const int InChannels)
{
    TexureId = CreateRandomTexture(InWidth, InHeight, InChannels);
}

OpenGL_Texture::~OpenGL_Texture()
{

    glDeleteTextures(1, &TexureId);
}

void OpenGL_Texture::Bind(GLuint TextureSlotID)
{
    glActiveTexture(TextureSlotID);
    glBindTexture(GL_TEXTURE_2D, TexureId);
}


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

GLuint CreateRandomTexture(const int Width, const int Height, const int Channels)
{
    
    unsigned char* Data = new unsigned char[Width * Height * Channels];
    for (int i = 0; i < Width * Height * Channels; ++i) {
        Data[i] = static_cast<unsigned char>(rand() % 256);
    }

    GLenum Format = Channels ==  3 ? GL_RGB : GL_RGBA;      //se l'immagine ha 3 canali è un RGB, se ne ha 4, un RGBA

    GLuint TextureID;
    glGenTextures(1, &TextureID);       //Creiamo la texture e la bindiamo
    glBindTexture(GL_TEXTURE_2D,TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);   //carichiamo i dati della texture sulla gpu
    delete Data;

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