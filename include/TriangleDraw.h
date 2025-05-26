#pragma once
#include <glad/glad.h>
class TriangleDraw
{
public:
    TriangleDraw();
    ~TriangleDraw();
    void Update(float InDeltaTime);
private:
    GLuint ProgramID;
    
};
