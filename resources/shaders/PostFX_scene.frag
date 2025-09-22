#version 460 core

out vec4 frag_color;

layout (binding = 0) uniform sampler2D storm_tex;

in vec2 vert_uv_out;

//Ill. di Phong -> prendiamo la posizione della pointLight, verrà caricata dal programma
uniform vec3 point_light_pos;


//Illuminazione di Phong -> prendiamo dal vertexShader le info su world_normal e world_position
in vec3 vert_world_normal_out;
in vec3 vert_world_position_out;



void main() 
{
    //frag_color = texture(storm_tex, vert_uv_out);

    //ILLUMINAZIONE DI PHONG ->AMBIENT - DIFFUSE - SPECULAR
    vec3 pixel_color = texture(storm_tex, vert_uv_out).xyz;     //prepariamo il pixel colorato di base con il pt corrisp dello stormtrooper
    
    //Ambient light
    float ambient_factor = 0.2f;
    vec3 ambient = pixel_color * ambient_factor;    //calcoliamo l'ambient sul pixel

    //Diffuse light
    vec3 light_dir = normalize(point_light_pos - vert_world_position_out);
    //Lambert -> world normal moltiplicato (dot product) per la directional  light (solo la comp positiva)
    float lambert = max(dot( normalize(vert_world_normal_out), light_dir), 0.f);        //fix di una dimenticanza. bisogna normalizzare i vert_normal
    vec3 diffuse = pixel_color * lambert;

    //TODO SPECULAR

    vec3 phong  = vec3(0, 0, 0);
    phong = ambient + diffuse;

    frag_color = vec4(phong, 1);
}