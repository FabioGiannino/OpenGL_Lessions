#version 460 core

out vec4 frag_color;

layout (binding = 0) uniform sampler2D scene_texture;

in vec2 vert_uv_out;


void main() 
{
    //frag_color = texture(scene_texture, vert_uv_out);

    /*GrayScale
    vec4 color = texture(scene_texture, vert_uv_out);
    float avg = (color.r + color.g + color.b)/5.0;
    frag_color = vec4(avg, avg, avg, 1.0f);
    */

    /*Negative
    vec4 color = texture(scene_texture, vert_uv_out);
    frag_color = 1.f - color;
    */

    //Black Band
    //gl_FragCoord è una funzione che ti permette di conoscere la pos del pixel corrente
    if (gl_FragCoord.y < 30 || gl_FragCoord.y > 570)
    {
        frag_color = vec4(0);   //se il pixel è in alto (<30) o in basso (>570), coloralo di nero
    }
    else
    {
        frag_color = texture(scene_texture, vert_uv_out);   //altrimenti coloralo secondo la texture
    } 


}