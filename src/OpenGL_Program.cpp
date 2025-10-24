/*Costruzione di una classe contenete le funzioni più comuni usate per comunicare con la GPU, come il compilatore dei file .vert e .frag o la creazione della pipeline*/
#include "OpenGL_Program.h"
#include <iostream>
#include <fstream>
#include <vector>

static std::string ReadFile(const std::string& InPath);
static GLuint CreateShader(const std::string& InPath, GLuint InShaderType);
static GLuint CreateProgram(GLuint VertexShaderID, GLuint FragmentShaderID);


OpenGL_Program::OpenGL_Program(const std::string& InVertexPath, const std::string& InFragmentPath)
{
    //creiamo gli shaders mandando il contenuto dei file triangle.vert e triangle.frag
    GLuint VertexShaderID = CreateShader(InVertexPath, GL_VERTEX_SHADER);
    GLuint FragmentShaderID = CreateShader(InFragmentPath, GL_FRAGMENT_SHADER);
    
    //Creiamo il programma/La pipeline dove carichiamo gli shaders
    ProgramID = CreateProgram(VertexShaderID, FragmentShaderID);
}

OpenGL_Program::~OpenGL_Program()
{
    glDeleteProgram(ProgramID);
}

void OpenGL_Program::Bind()
{
    glUseProgram(ProgramID);
}

//Creiamo una funzione per leggere i file .vert e .frag, dove sono contenuti gli shaders 
std::string ReadFile(const std::string& InPath)
{
    std::ifstream InputStream(InPath,std::ios::ate);    //apro il file e sposto il cursore direttamente alla fine
    size_t FileSize = InputStream.tellg();      //Leggo quanto è grande (in byte) il cursore -> ergo, a che punto sono arrivato -> ergo, essendo alla fine, la dimesione del file

    //Ora che sappiamo quanto è grande il file, creiamo un buffer [una semplice stringa, che alla fine è un puntatore]
    std::string Text;   
    Text.resize(FileSize);  //e ne settiamo la grandezza pari al file

    InputStream.seekg(0, std::ios::beg);       //riportiamo il cursore a 0, dicendogli di portare il cursore al begin e leggere il carattere all'offset 0
    InputStream.read(&Text[0], FileSize);   //scriviamo il file nella variabile Text
     
    InputStream.close();
    return Text;
}

//Una volta letto il file .vert o .frag, si crea lo shader caricando in GPU la sequenza contenuta nei file, che ritorna un identificativo 
//sotto forma di GL Unsigned Int
GLuint CreateShader(const std::string& InPath, GLuint InShaderType)
{
    std::string Text = ReadFile(InPath);
    const char* ShaderSource = Text.c_str();

    GLuint ShaderID = glCreateShader(InShaderType);     //Creiamo lo shader, dicendogli il tipo (Se Vertex o Fragment)
    glShaderSource(ShaderID, 1, &ShaderSource, NULL);   //carichiamo le istruzioni lette dal file .vert/.frag -> da notare l'ultimo parametro indica quante è la lunghezza delle istruzioni da leggere [NULL per dire tutto il file]
    glCompileShader(ShaderID);          //Avviamo la compilazione dello shader ->!!Se non funzia, non abbiamo modo di saperlo!!

    GLint Success;
    glGetShaderiv(ShaderID,GL_COMPILE_STATUS, &Success); //DEBUG per la compilazione, ci scriviamo sulla var Success se lo shader è stato compilato correttamente
    if(!Success)//Se non è andato bene, vediamo di stampare gli errori...
    {
        
        GLint MaxLogLenght;
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &MaxLogLenght); //Ottengo la lunghezza dell'InfoLog
        
        std::vector<GLchar> InfoLog(MaxLogLenght);      
        glGetShaderInfoLog(ShaderID, MaxLogLenght, NULL, InfoLog.data()); //Inserisco tutto il log dentro la memoria di un vettore
        
        std::string LogString(InfoLog.begin(), InfoLog.end());  //Costruiamo la stringa da stampare a console
        std::cout << "[ERRO]: Shader Compilation Failure: " << LogString <<std::endl;
        throw std::runtime_error(LogString);        //Stampiamo e lanciamo l'eccezione
    }
    
    return ShaderID;    //se tutto è andato bene invece, ritorniamo l'id 
}

//Creazione del programma per caricare gli shaders creati sopra (tipo una pipeline)
GLuint CreateProgram(GLuint VertexShaderID, GLuint FragmentShaderID)
{
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID,VertexShaderID);
    glAttachShader(ProgramID,FragmentShaderID);
    glLinkProgram(ProgramID);

    /*Copiamo da sopra il DEBUG per stampare i log e verificare cosa è andato storto*/
    GLint Success;
    glGetProgramiv(ProgramID ,GL_LINK_STATUS, &Success); 
    if(!Success)
    {        
        GLint MaxLogLength;
        glGetShaderiv(ProgramID, GL_INFO_LOG_LENGTH, &MaxLogLength); //Ottengo la lunghezza dell'InfoLog
        
        std::vector<GLchar> InfoLog(MaxLogLength);      
        glGetProgramInfoLog(ProgramID, MaxLogLength, NULL, InfoLog.data()); //Inserisco tutto il log dentro la memoria di un vettore
        
        std::string LogString(InfoLog.begin(), InfoLog.end());  //Costruiamo la stringa da stampare a console
        std::cout << "[ERRO]: Program Compilation Failure: " << LogString <<std::endl;
        throw std::runtime_error(LogString);        //Stampiamo e lanciamo l'eccezione
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    

    return ProgramID;
}

GLuint OpenGL_Program::GetProgramID() const
{
    return ProgramID;
}

//Come detto nell'header, queste due funzioni di wrap sono un'alternativa per non esporre il ProgramID, anche se in realtà con un get va bene lo stesso
void OpenGL_Program::SetUniform(const std::string& InName, float InValue)
{
    glUniform1f(glGetUniformLocation(ProgramID, InName.c_str()), InValue);
}

void OpenGL_Program::SetUniform(const std::string& InName, int InValue)
{
    glUniform1i(glGetUniformLocation(ProgramID, InName.c_str()), InValue);
}

void OpenGL_Program::SetUniform(const std::string& InName, const Color& InValue)
{
    glUniform4fv( glGetUniformLocation(ProgramID, InName.c_str()), 
        1, reinterpret_cast<const GLfloat*>(&InValue));
}

void OpenGL_Program::SetUniform(const std::string &InName, const glm::mat4 &InValue)
{
    glUniformMatrix4fv( glGetUniformLocation(ProgramID, InName.c_str()), 
        1,  GL_FALSE, &InValue[0][0]);
}
void OpenGL_Program::SetUniform(const std::string &InName, const glm::vec3 &InValue)
{
    glUniform3fv( glGetUniformLocation(ProgramID, InName.c_str()), 
        1,  &InValue[0]);  
}