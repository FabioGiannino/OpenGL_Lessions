#version 460 core

uniform float time;
out vec4 frag_color;

layout (binding = 0) uniform sampler2D scene_texture;

in vec2 vert_uv_out;

//--------------------------------------------------------------------------
//Funzione di blur: si prende il colore dei pixel vicini e si fa una media per sfocare
//  per "vicini" si intende un parametro che dipende dalla dimensione della scena
vec4 blur()         
{
    vec2 texture_size = textureSize(scene_texture, 0);  //funzione per avere la dimensione della texture in entrata
    float uv_w = 1.f/texture_size.x;
    float uv_h = 1.f/texture_size.y;

    vec4 contrib = vec4(0);             //la forza del blur

    //taking 3x3 pixel Matrix
    for (int i = -3 ; i <= 3; ++i)
    {
        float offset_x = uv_w * i;

        for(int j = -3; j<= 3 ; j++)
        {
            float offset_y = uv_h * j;
            vec2 uv_current = vec2(vert_uv_out.x + offset_x , vert_uv_out.y + offset_y);
            contrib += texture(scene_texture, uv_current);          //una sottosezione della texture originale
        }
    }

    vec4 result = contrib / 49;  //faccio la media dei colori
    return result;
}   
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
//Funzione "onda": sfruttiamo la funzione seno per campionare le uv della texture, cambiandole così in maniera "ondulata"
//Funzione onda statica. si modificano le uv in base a valori statici
vec4 static_wave()
{
    //funz seno --> y = A*sin(B*(x+C)) + D      con A = Ampiezza, B = Fase, C = Spost. sinistra, D = Spost. verticale
    
    float A = 1.f/ 70.f;
    float B = 20.f;
    float C = 1.f / 40.f;
    float D = 0.f;

    vec2 uv_current = vert_uv_out;
    uv_current.x += A*sin(B*(uv_current.y + C)) + D;
    return texture(scene_texture, uv_current); 
}
//Funzione onda Dinamica. le UV si modificano con il tempo
vec4 dynamic_wave()
{
    //funz seno --> y = A*sin(B*(x+C)) + D      con A = Ampiezza, B = Fase, C = Spost. sinistra, D = Spost. verticale
    
    float A = 1.f/ 70.f;
    float B = 20.f;
    float C = time / 40.f;
    float D = 0.f;

    vec2 uv_current = vert_uv_out;
    uv_current.x += A*sin(B*(uv_current.y + C)) + D;
    return texture(scene_texture, uv_current); 
}
//--------------------------------------------------------------------------




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
    /*
    if (gl_FragCoord.y < 30 || gl_FragCoord.y > 570)
    {
        frag_color = vec4(0);   //se il pixel è in alto (<30) o in basso (>570), coloralo di nero
    }
    else
    {
        frag_color = texture(scene_texture, vert_uv_out);   //altrimenti coloralo secondo la texture
    } 
    */
    //Black Band 2° versione: non con le coordinate ma con le uv
    /*
    if( vert_uv_out.y < 0.1 || vert_uv_out.y > 0.9)
    {
        frag_color = vec4(0);
    }
    else
    {
        frag_color = texture(scene_texture, vert_uv_out);
    }
    */
    
    //frag_color = blur();

    frag_color = dynamic_wave();
}