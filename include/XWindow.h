//Creiamo una classe apposta per gestire tutte le funzioni della finestra di OpenGL
#include <string>

/*Spiegazione di questa riga: Attualmente questo file header NON conosce il tipo GLFWwindow. Per risolverlo dovremmo importare GLFW/glfw3.h,
 Però in generale nei file .h si cerca di importare meno librerie possibili, questo perchè quando vengono linkate ai file cpp, vengono proprio copiate e incollate.
 Quindi se un file header viene usato da tanti file cpp, è consigliabile che tale file sia LEGGERO
- Dovremmo quindi creare una classe, tuttavia GLFW è costruito in C, che non riconosce le classi. Allora si crea una struttura con lo stesso nome così da far contento
il compilatore*/
typedef struct GLFWwindow GLFWwindow;

class XWindow
{
public:
    
    XWindow(int InWidth, int InHeight, std::string InTitle); //Costruttore
    ~XWindow();

    bool IsOpened() const;
    void SetTitle( const std::string InTitle);
    void Update();
    float GetDeltaTime() const;
    void SetVSync(bool InEnabled);


private:
    int Width;
    int Height;
    std::string Title;
    GLFWwindow* RawWindow;
    float DeltaTime;
};