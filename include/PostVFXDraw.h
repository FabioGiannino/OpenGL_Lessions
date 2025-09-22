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
    GLuint SceneVao;
    GLuint Vbo;
    GLuint Ebo;
    class OpenGL_Program* SceneProgram;
    class OpenGL_Texture* StormTextureId;
    glm::mat4 View;
    glm::mat4 Projection;
    std::vector<float> SceneVertices;

    GLuint SceneFbo;
    GLuint SceneTexture;
    GLuint SceneRbo;

    class OpenGL_Program* FXProgram;
    GLuint FxVao;
    GLuint FxVbo;
};