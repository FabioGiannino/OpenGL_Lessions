#pragma once
#include <glad/glad.h>
class QuadIndexDraw
{
public:
    QuadIndexDraw();
    ~QuadIndexDraw();
    void Update(float InDeltaTime);
private:
    GLuint ProgramID;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
};
