#version 120

varying vec3 normal;
varying vec4 vertex_3d_original;
varying vec4 vertex_3d_modelview;

uniform sampler2D texture;
uniform int object_id;


vec3 light=vec3(0.5,3,6);

void main (void)
{
    vec3 n=normalize(normal);

    vec3 p=vertex_3d_modelview.xyz;
    vec3 vertex_to_light=normalize(light-p);
    vec3 reflected_light=reflect(-vertex_to_light,n);
    vec3 user_to_vertex=normalize(-p);

    float diffuse_term=0.7*clamp(dot(n,vertex_to_light),0,1);
    float specular_term=0.2*pow(clamp(dot(reflected_light,user_to_vertex),0,1),128);
    float ambiant_term=0.75;

    vec4 white=vec4(1,1,1,0);
    vec4 color = vec4(0.11,0.44,0.71,1.0);

    vec2 tex_coord=gl_TexCoord[0].xy;
    if(object_id==0){ //Texture
         color =texture2D(texture,tex_coord);
        
    }
    else
    {
        if(object_id ==1){ //Rouge pour le cube
            color = vec4(1.0,0.0,0.0,1.0);
        }
        else
        {
            if (object_id==2) {//texte
                color= vec4(86.0/255.0, 126.0/255.0, 149.0/255.0,1.0);
            }
            else
            {
                color = vec4(0.11,0.44,0.71,1.0); //vaisseau

            }
            
        }


    }
    gl_FragColor = (ambiant_term+diffuse_term)*color+specular_term*white;
}
