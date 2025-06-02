#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_uv;

out vec2 vert_uv_out;

//Definiamo un'altra funzione per calcolarci l'aspect ratio
vec3 perspective(vec3 pos)
{
    float aspect_ratio = 800.f /600.f;
    float vertical_fov = 60.f;
    float fov = tan(radians(vertical_fov * 0.5f));      //calcolo del fov -> la tangente della metà dell'angolo di fov (convertito in radianti)

    vec3 result;
    result.x = pos.x / (fov * aspect_ratio);
    result.y = pos.y / fov;
    result.z = pos.z;
    return result;
}

void main() 
{
    vec3 fixed_vert = perspective(vert_pos);        //richiamiamo nel main l'altra funzione
    gl_Position = vec4(fixed_vert, 1.f);
    vert_uv_out = vert_uv;
}