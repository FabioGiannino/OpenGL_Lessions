#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>
#include "XWindow.h"
#include "TriangleDraw.h"
#include "QuadDraw.h"
#include "QuadIndexDraw.h"
#include "QuadColorDraw.h" 
#include "TextureDraw.h"	
#include "TexurePerspective.h"
#include "CubeDraw.h"
#include "RandomTextureDraw.h"

//necessario per forzare l'uso della scheda video NVidia sul laptop invece della scheda video integrata Intel
#ifdef _WIN32
extern "C" {
	   __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif	

int main() {
	
	XWindow Win = XWindow(800, 600, "OPENGL Window");
	

	//Queste due variabili servono per stabilizzare il titolo della finestra, se no si aggiorna ogni frame.
	float TitleUpdateMaxTime = 1.f;			//aggiorniamo il titolo una volta al secondo
	float TitleUpdateElapsed = 0.f;			//Contatore
	
	//TriangleDraw Scene;
	//QuadDraw Scene;
	//QuadIndexDraw Scene;
	//QuadColorDraw Scene;
	//TextureDraw Scene;
	//TexurePerspective Scene;
	//CubeDraw Scene;
	RandomTextureDraw Scene;
	

	//A questo punto, abbiamo la finestra, ma partirà e si chiuderà all'istante. Creiamo il while per evitare che si chiuda
	while(Win.IsOpened())
	{
		
		float DeltaTime = Win.GetDeltaTime();		

		//Mostriamo il titolo una volta al secondo, usando un contatore che si incrementa del delta time e quando arriva a 1 mostra il titolo
		TitleUpdateElapsed+= DeltaTime;
		if(TitleUpdateElapsed >= TitleUpdateMaxTime)
		{
			int Fps = 1.f / DeltaTime;
			std::string formattedTitle =  std::format("OpenGL App | DeltaTime =  {}    FPS = {}", DeltaTime, Fps);
			Win.SetTitle(formattedTitle);
			TitleUpdateElapsed -= TitleUpdateElapsed; //Invece che mettere "= 0", sottraiamo il contatore a 1 così da avere anche il residuo possibile
		}

		Scene.Update(DeltaTime);
		
		//Aggiorniamo la finestra 
		Win.Update();

	}
	
	return 0;	
}