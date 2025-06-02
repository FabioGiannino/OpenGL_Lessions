#pragma once
#include <string>
#include <glad/glad.h>

class OpenGL_Texture
{
public:
    OpenGL_Texture(const std::string& InImagePath);
    ~OpenGL_Texture();
    void Bind(GLuint TextureSlotID);
private:
    GLuint TexureId;
};