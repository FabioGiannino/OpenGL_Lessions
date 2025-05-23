#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>

#ifdef _WIN32
extern "C" {
	   __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif	

int main() {
	
	
	/*Come prima cosa creiamo il contesto grafico
	Iniziamo verificando che GLFW funzioni*/

	if(glfwInit() == GLFW_FALSE) {
		std::cerr << "GLFW initialization failed!" << std::endl;
		return -1;
	}

	//GLFW è il nostro wrapper per il contesto grafico. Quindi indirizziamolo per il contesto OpenGL, che è alla versione 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE); //Eseguirlo in modalità compatibilità per le versioni precedenti
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //è un profilo core "stretto" che non include le funzioni obsolete
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); //Abilitiamo il contesto di debug per vedere gli errori 

	//Ora andiamo a creare la finestra dove girerà il programma, creando tutte le variabili
	GLFWmonitor* monitor = NULL; 		//Iniziamo settando il monitor. Mettendo NULL verrà preso il monitor principale
	int width = 640;
	int height = 480;
	const char* title = "OPENGL Window";
	GLFWwindow* window = glfwCreateWindow(width,height,title,monitor, NULL);	//creiamo la finestra
	if(window == NULL)
	{
		std::cerr << "[ERR]: Window not created" << std::endl;
		throw std::runtime_error("[ERR]: Window not created");
	}
	glfwMakeContextCurrent(window);	
	if (gladLoadGL() == 0)		//Carichiamo le funzioni di OpenGL, controllando che si siano caricate
	{
		std::cerr << "[ERR]: Error loading GL Functions" << std::endl;
		throw std::runtime_error("[ERR]: Error loading GL Functions");
	}



	//A questo punto, abbiamo la finestra, ma partirà e si chiuderà all'istante. Creiamo il while per evitare che si chiuda
	float DeltaTime = 0.f;
	while(!glfwWindowShouldClose(window))
	{
		static float LastTime = glfwGetTime();
		
		float CurrTime = glfwGetTime();
		DeltaTime = CurrTime - LastTime;
		LastTime = CurrTime;

		int Fps = 1.f / DeltaTime;
		std::string formattedTitle =  std::format("OpenGL App | DeltaTime =  {}    FPS = {}", DeltaTime, Fps);

		glfwSetWindowTitle(window, formattedTitle.c_str());

		//Ok la finestra è aperta, ma in questo modo avremo solo un patacco unicolore FREEZATO. Dobbiamo "consumare" gli eventi della finestra per aggiornalaù
		glfwSwapBuffers(window);		//Swap del front buffer con il back buffer
		glfwPollEvents();				//consumo degli eventi
	}



	return 0;	
}