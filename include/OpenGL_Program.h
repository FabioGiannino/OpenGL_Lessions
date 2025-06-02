#pragma once
#include <string>
#include <glad/glad.h>
#include "XCommon.h"

class OpenGL_Program
{
public:
    OpenGL_Program(const std::string& InVertexPath, const std::string& InFragmentPath);
    ~OpenGL_Program();
    void Bind();
    GLuint GetProgramID() const;
    
    //Anche se abbiamo inserito il GetProgramID, mostriamo un altro modo per wrappare nella classe Program i vari glUniform.
    // Sfruttiamo l'overloading  dei metodi, usando i tipi di variabili ad-hoc per la glUniform
    void SetUniform(const std::string &InName, float InValue);
    void SetUniform(const std::string &InName, const Color &InValue);
    
private:
    GLuint ProgramID;
    
};