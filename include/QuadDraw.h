#pragma once
#include <glad/glad.h>
class QuadDraw
{
public:
    QuadDraw();
    ~QuadDraw();
    void Update(float InDeltaTime);
private:
    GLuint ProgramID;
    GLuint VAO;
    GLuint VBO;
    
};
