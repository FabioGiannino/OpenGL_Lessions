#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_uv;
layout (location = 2) in vec3 vert_norm;

uniform mat4 mvp;

out vec2 vert_uv_out;

//Illuminazione di Phong -> passiamo al fragmentShader le info su world_normal e world_position
out vec3 vert_world_normal_out;
out vec3 vert_world_position_out;

//problema: i vertici che abbiamo vengono dal file obj, che è in coordinate Local.
//dobbiamo passare la matrice Model per ottenere le coordinate in global space
uniform mat4 model;

void main() 
{
    gl_Position = mvp * vec4(vert_pos, 1.0f);
    vert_uv_out = vert_uv;

    //da localSpace a worldSpace 
    vert_world_position_out = (model * vec4(vert_pos, 1)).xyz;     //questo accrocchio è necessario perchè model è una mat 4x4, e non si può moltiplicare per un vec3 per ottenere un vec3
    vert_world_normal_out = (model * vec4(vert_norm, 1)).xyz;

}