#pragma once
#include <glad/glad.h>

class CubeDraw 
{
public:
    CubeDraw();
    ~CubeDraw();
    void Update(float InDeltaTime);
private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;
    class OpenGL_Program* Program;
    class OpenGL_Texture* SmileTextureId;
    class OpenGL_Texture* BoxTextureId;
};