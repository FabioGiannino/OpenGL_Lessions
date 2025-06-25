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
    //vert_world_normal_out = (model * vec4(vert_norm, 1)).xyz;

    /*
    Piccolo discorso sulle normali: C'è un problema con il calcolo sopra, e cioè che quando sono applicati degli Scale sulla figura, 
    la normale viene "schiacciata" in quanto è calcolata sul punto non scalato. Immaginiamo un cerchio con le sue normali per i pt, ora scaliamolo lungo un asse 
    e otterremo un ellisse. Le normali del cerchio ai lati si trasfomano e vengono anche loro scalati, ma mantengono la stessa direzione del cerchio, non dell'ellise.
    così avremo delle normali che non sono normali! 
    Di conseguenza dobbiamo calcolare le normali per il mondo in modo diverso. Premesso che la rotazione funziona e che la traslazione non ci serve (sono normali, a noi basta
    la loro direzione, se si traslano non cambia), dobbiamo invertire la matr model per ottenere le nuove normali. Qual è il problema? che così facendo anche le possibili rotazioni
    vengono rotte. Qui però entra in gioco una proprietà della matr rotazione, che dice che la trasposta dell'inversa è uguale alla matr. rotazione originale.
    Quindi, applicando l'inversa, e poi la trasposta, otteniamo che la rotazione non cambia per quello detto sopra, la trasposta non modifica lo scale perchè lo scale è sulla diagonale,
    l'unica cosa che si modifica è la traslazione (sull'ultima colonna), ma non ci serve, quindi la eliminiamo passando da un mat4 a un mat3 
    */
    vert_world_normal_out = mat3(transpose(inverse(model))) * vert_norm;
}