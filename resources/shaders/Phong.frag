#version 460 core

out vec4 frag_color;

layout (binding = 0) uniform sampler2D storm_tex;

in vec2 vert_uv_out;

//Ill. di Phong -> prendiamo la posizione della pointLight, verrà caricata dal programma
uniform vec3 point_light_pos;


//Illuminazione di Phong -> prendiamo dal vertexShader le info su world_normal e world_position
in vec3 vert_world_normal_out;
in vec3 vert_world_position_out;

//Specular -> passiamo la posizione della camera

uniform vec3 camera_position;


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

    //SPECULAR ->Ripassino, la specular si calcola conoscendo il versore della luce riflessa sul pt e il versore del punto alla camera.
    //  si fa il dot product tra i due e poi si moltiplica per un intensità (che dipenderebbe dal material, ma qui è stato messo un val arbitrario)
    
    //calcolo del vettore punto-camera
    vec3 dir_to_eye = normalize(camera_position - vert_world_position_out);
    //calcolo del vettore luce riflessa 
    vec3 light_dir_to_point = light_dir * -1.f; //luce sul punto 
    vec3 light_reflect = reflect(light_dir_to_point,  normalize(vert_world_normal_out)); //mirror rispetto alla normale del punto

    float cosER = dot(dir_to_eye, light_reflect);   //dot product
    float specular_value =  max(cosER, 0.f);        //solo valori positivi
    vec3 specular_color = vec3(1,1,1);              //colore del riflesso bianco
    vec3 specular = specular_color * pow(specular_value, 50.f); 




    vec3 phong  = vec3(0, 0, 0);
    phong = ambient + diffuse + specular;
    //phong = ambient + diffuse;

    frag_color = vec4(phong, 1);
}