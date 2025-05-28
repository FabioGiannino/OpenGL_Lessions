#include "QuadIndexDraw.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

//In questa nuova versione del QuadDraw, invece di disegnare due triangoli distinti partendo da sei vertici, si disegneranno quattro vertici e li si indicizzano

/*Una cosa che si nota di più nella rasterizzazione del quadrato rispetto a quella del triangolo è la mancanza di un fix per l'aspect Ratio
Difatti si nota come il risultato finale è più un rettangolo, a causa del fatto che si espande in base alla grandezza della viewport*/


//Creiamo una funzione per leggere i file .vert e .frag, dove sono contenuti gli shaders 
static std::string ReadFile(const std::string& InPath)
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
static GLuint CreateShader(const std::string& InPath, GLuint InShaderType)
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
static GLuint CreateProgram(GLuint VertexShaderID, GLuint FragmentShaderID)
{
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID,VertexShaderID);
    glAttachShader(ProgramID,FragmentShaderID);
    glLinkProgram(ProgramID);

    /*Copiamo da sopra il DEBUG per stampare i log e verificare cosa è andato storto*/
    GLint Success;
    glGetShaderiv(ProgramID ,GL_COMPILE_STATUS, &Success); 
    if(!Success)
    {        
        GLint MaxLogLenght;
        glGetShaderiv(ProgramID, GL_INFO_LOG_LENGTH, &MaxLogLenght); //Ottengo la lunghezza dell'InfoLog
        
        std::vector<GLchar> InfoLog(MaxLogLenght);      
        glGetShaderInfoLog(ProgramID, MaxLogLenght, NULL, InfoLog.data()); //Inserisco tutto il log dentro la memoria di un vettore
        
        std::string LogString(InfoLog.begin(), InfoLog.end());  //Costruiamo la stringa da stampare a console
        std::cout << "[ERRO]: Shader Compilation Failure: " << LogString <<std::endl;
        throw std::runtime_error(LogString);        //Stampiamo e lanciamo l'eccezione
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    

    return ProgramID;
}


//In questo caso, disegneremo il quadrato usando solo quattro vertici, a differenza dei sei per i due triangoli
// Sarà necessario usare un altro buffer ad-hoc per i vertici dei triangoli
QuadIndexDraw::QuadIndexDraw()
{
    //creiamo gli shaders mandando il contenuto dei file triangle.vert e triangle.frag
    GLuint VertexShaderID = CreateShader("resources/shaders/triangle.vert", GL_VERTEX_SHADER);
    GLuint FragmentShaderID = CreateShader("resources/shaders/triangle.frag", GL_FRAGMENT_SHADER);
    
    //Creiamo il programma/La pipeline dove carichiamo gli shaders
    ProgramID = CreateProgram(VertexShaderID, FragmentShaderID);

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
    glUseProgram(ProgramID);                
}

QuadIndexDraw::~QuadIndexDraw()
{   
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteProgram(ProgramID);
}

void QuadIndexDraw::Update(float InDeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);   //Ad ogni update puliamo il buffer 
    //glDrawArrays(GL_TRIANGLES, 0, 6);   
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);   //!Usando DrawElements invece che DrawArray, la GPU Andrà a leggere all'interno dell'EBO creata
            //^^ La EBO poi dirà alla GPU di mostrare gli elementi dentro la VAO secondo gli indici specificati
}