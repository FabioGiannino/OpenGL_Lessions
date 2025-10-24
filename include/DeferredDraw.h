#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp> 
#include <vector>

class DeferredDraw 
{
public:
    DeferredDraw();
    ~DeferredDraw();
    void Update(float InDeltaTime);
private:
    GLuint SceneVao;
    GLuint Vbo;
    GLuint Ebo;
    class OpenGL_Program* SceneProgram;
    class OpenGL_Texture* StormTextureId;
    glm::mat4 View;
    glm::mat4 Projection;
    std::vector<float> SceneVertices;

    GLuint GFbo;
    GLuint DiffuseTexture;
    GLuint NormalTexture;
    GLuint PositionTexture;
    GLuint SceneRbo;

    class OpenGL_Program* BlendingProgram;
    GLuint BlendingVao;
    GLuint BlendingVbo;
};