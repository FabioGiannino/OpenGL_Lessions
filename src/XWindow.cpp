#include "XWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream> 
#include <stdexcept>

//Costruttore
XWindow::XWindow(int InWidth, int InHeight, std::string InTitle)
    : Width(InWidth), Height(InHeight), Title(InTitle)  //Initialization list
	, RawWindow(nullptr), DeltaTime(0.f)
{
    /*Come prima cosa creiamo il contesto grafico
	Iniziamo verificando che GLFW funzioni*/
	if(glfwInit() == GLFW_FALSE) {
		std::cerr << "GLFW initialization failed!" << std::endl;
        throw std::runtime_error("[ERR]: GLFW initialization failed!");
		return ;
	}

	//GLFW è il nostro wrapper per il contesto grafico. Quindi indirizziamolo per il contesto OpenGL, che è alla versione 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE); //Eseguirlo in modalità compatibilità per le versioni precedenti
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //è un profilo core "stretto" che non include le funzioni obsolete
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); //Abilitiamo il contesto di debug per vedere gli errori 

	//Ora andiamo a creare la finestra dove girerà il programma, creando tutte le variabili
	GLFWmonitor* monitor = NULL; 		//Iniziamo settando il monitor. Mettendo NULL verrà preso il monitor principale

	RawWindow = glfwCreateWindow(Width,Height, Title.c_str(), monitor, NULL);	//creiamo la finestra
	if(RawWindow == NULL)
	{
		std::cerr << "[ERR]: Window not created" << std::endl;
		throw std::runtime_error("[ERR]: Window not created");
	}
	glfwMakeContextCurrent(RawWindow);	
	if (gladLoadGL() == 0)		//Carichiamo le funzioni di OpenGL, controllando che si siano caricate
	{
		std::cerr << "[ERR]: Error loading GL Functions" << std::endl;
		throw std::runtime_error("[ERR]: Error loading GL Functions");
	}
}

//Dopo che abbiamo creato la window, è necessario anche distruggerla alla fine della sua vita, per liberare memoria
XWindow::~XWindow()
{
	glfwDestroyWindow(RawWindow);
}

bool XWindow::IsOpened() const
{
    return !glfwWindowShouldClose(RawWindow);
}

void XWindow::SetTitle( const std::string InTitle)
{
	glfwSetWindowTitle(RawWindow, InTitle.c_str());
}

//Poichè lo swap del buffer e il consumo degli eventi della finestra vengono fatti a ogni frame,
//tantovale dichiararli in una funzione Update che verrà richiamata finchè la finestra è aperta
void XWindow::Update()
{
	//Aggiorniamo anche ad ogni frame il suo delta time
	static float LastTime = glfwGetTime();
	float CurrTime = glfwGetTime();
	DeltaTime = CurrTime - LastTime;
	LastTime = CurrTime;

	glfwSwapBuffers(RawWindow);		//Swap del front buffer con il back buffer
	glfwPollEvents();				//consumo degli eventi
}


float XWindow::GetDeltaTime() const
{
	return DeltaTime;
}

/*Di default, il Vsync è attivo, cioè lo swap buffer della gpu è sincronizzato con la frequenza di aggiornamento dello schermo
  Se si volesse disattivare tale comportamento, bisogna mettere a 0 l'intervallo di swap dei buffer */
void XWindow::SetVSync(bool InEnabled)
{
	if(!InEnabled)
	{
		glfwSwapInterval(0);
	}
	else
	{
		glfwSwapInterval(1);
	}
}