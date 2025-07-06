#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp> 
#include <vector>

class PostVFXDraw 
{
public:
    PostVFXDraw();
    ~PostVFXDraw();
    void Update(float InDeltaTime);
private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;
    class OpenGL_Program* Program;
    class OpenGL_Texture* StormTextureId;
    glm::mat4 View;
    glm::mat4 Projection;
    std::vector<float> Vertices;
};