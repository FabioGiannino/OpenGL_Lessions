#pragma once
#include <string>
#include <glad/glad.h>

class OpenGL_Program
{
public:
    OpenGL_Program(const std::string& InVertexPath, const std::string& InFragmentPath);
    ~OpenGL_Program();
    void Bind();
    GLuint GetProgramID() const;
private:
    GLuint ProgramID;
    
};