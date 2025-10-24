#version 460 core

out vec4 frag_color;

layout (binding = 0) uniform sampler2D diffuse_tex;
layout (binding = 1) uniform sampler2D normal_tex;
layout (binding = 2) uniform sampler2D position_tex;

in vec2 vert_uv_out;
uniform vec3 camera_position;


//In GPU non possiamo allocare memoria dinamicamente, di conseguenza per gestire un array di luci
//Dobbiamo istanziare un numero molto grande di luci e poi passargli quante luci stiamo realmente usando
const int MAX_NUM_LIGHTS = 1000;
uniform vec3 point_light_pos[MAX_NUM_LIGHTS];
uniform int light_count;                //l'effettivo numero di luci presenti in scena


vec3 compute_phong(int light_index)
{
    vec3 pixel_color = texture(diffuse_tex, vert_uv_out).xyz;

    //Ambient
    float ambient_factor = 0.2f;
    vec3 ambient = pixel_color * ambient_factor;

    //Diffuse 
    vec3 vert_world_position_out = texture(position_tex, vert_uv_out).xyz;      //--> prendendo la position texture si individuano i vertici su cui calcolare le luci!
    vec3 vert_world_normal_out = texture(normal_tex, vert_uv_out).xyz;

    vec3 light_dir = normalize(point_light_pos[light_index] - vert_world_position_out);
    float lambert = max(dot( normalize(vert_world_normal_out) , light_dir ), 0.0f);
    vec3 diffuse = pixel_color * lambert;

    //Specular todo
    vec3 dir_to_eye = normalize(camera_position - vert_world_position_out);    
    vec3 light_dir_to_point = light_dir * -1.f; 
    vec3 light_reflect = reflect(light_dir_to_point,  normalize(vert_world_normal_out)); 
    float cosER = dot(dir_to_eye, light_reflect);   
    float specular_value =  max(cosER, 0.f);        
    vec3 specular_color = vec3(1,1,1);              
    vec3 specular = specular_color * pow(specular_value, 50.f); 
    
    //Phong
    vec3 phong = vec3(0, 0, 0);
    phong += ambient;
    phong += diffuse;
    phong += specular;
    return phong;
}



void main()
{
    vec3 light_contrib = vec3(0);

    for (int i = 0; i< light_count; ++i)
    {
        light_contrib += compute_phong(i);
    }

    frag_color = vec4(light_contrib,1);
}