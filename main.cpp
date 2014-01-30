/*****************************************************************************\
 * TP CPE, 3ETI, TP synthese d'images
 * --------------
 *
 * Programme principal des appels OpenGL
\*****************************************************************************/



#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "string.h"

#define GLEW_STATIC 1
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <Opengl/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include "glm.h"
#include "glutils.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "triangle_index.hpp"
#include "image.hpp"
#include <sstream>




/*****************************************************************************\
 * Structure
 *
 *
\*****************************************************************************/

//structure d'un vertex OpenGL contenant une position, une normale, une couleur et une texture
struct vertex_opengl
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 texture;
};

enum lane {
    left = 0,
    middle = 1,
    right = 2
    };

/*****************************************************************************\
 * Variables globales
 *
 *
\*****************************************************************************/

//identifiant du shader
GLuint shader_program_id;
//identifiant texture
GLuint texture_id;
//identifiant d'objet
GLuint object_id;

//vertex buffer
GLuint vbo_cylinder,vboi_cylinder;
GLuint vbo_background,vboi_background;

//temps
int counter_time=0;
int counter_time_buff = 1;

int counter_collision = 0;

int seuil = 95;
int counter_seuil = 0;


int position_vaisseau = 0;



float position_cube = 0.0;

float speed = 0;

lane cube_lane = middle;
lane ship_lane = middle;
lane ship_previous_lane = middle;

vec3 angle;
vec3 deplacement;

GLMmodel * ship ;
GLMmodel * cube;


int w, h;
char score[30] = "";
double t = 0;
char vie[5]="";
int nbvie=3;
bool debug=0;
bool pause=0;
bool fin=0;

/*****************************************************************************\
 * Fonctions GLUT
 *
 *
\*****************************************************************************/

static void generate_background(){
    vertex_opengl vertex[4];
    
        vertex[0].position = vec3(0.0,0.0,0.0);
        vertex[1].position = vec3(0.0,2.0,0.0);
        vertex[2].position = vec3(5.0,0.0,0.0);
        vertex[3].position = vec3(5.0,2.0,0.0);

    
  
        vertex[0].texture = vec2(1.0,0.5);
        vertex[1].texture = vec2(1.0,0.0);
        vertex[2].texture = vec2(0.0,0.5);
        vertex[3].texture = vec2(0.0,0.0);
    
    
    
    //connectivite
    triangle_index index[2];
    index[0] = triangle_index(0,1,2);
    index[1] = triangle_index(1,2,3);
  
    //Envoie des donnees de "vertex" en memoire video
    glGenBuffers(1,&vbo_background); PRINT_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER,vbo_background); PRINT_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertex),vertex,GL_STATIC_DRAW); PRINT_OPENGL_ERROR();
    
    //Envoie des indices en memoire video
    glGenBuffers(1,&vboi_background); PRINT_OPENGL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_background);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);
}
static void generate_cylinder(){
    const int N_sommet = 128;

    vertex_opengl vertex[N_sommet];
    
    for(int i =0; i<N_sommet/2;i++){
        vertex[2*i].position = vec3(-0.8,cos(14.3*i/float(N_sommet)),sin(14.3*i/float(N_sommet)));
        vertex[2*i].color=vec3(1.0,0.0,0.0);
        vertex[2*i].normal=vec3(0,0,1);
        vertex[2*i+1].position = vec3(0.8,cos(14.3*i/float(N_sommet)),sin(14.3*i/float(N_sommet)));
        vertex[2*i].color=vec3(1.0,0.0,0.0);
        vertex[2*i+1].normal=vec3(0,0,1);
    }
    for(int i =0; i<N_sommet/4;i++){
        
        vertex[4*i].texture = vec2(0,2.0/3.0);
        vertex[4*i+1].texture = vec2(1,2.0/3.0);
        vertex[4*i+2].texture = vec2(0,3.0/3.0);
        vertex[4*i+3].texture = vec2(1,3.0/3.0);
    }
    
    //connectivite
    triangle_index index[N_sommet-3];
    for(int i =0;i<N_sommet-2;i++){
        index[i]=triangle_index(i,i+1,i+2);
    }
    
    //Envoie des donnees de "vertex" en memoire video
    glGenBuffers(1,&vbo_cylinder); PRINT_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER,vbo_cylinder); PRINT_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertex),vertex,GL_STATIC_DRAW); PRINT_OPENGL_ERROR();
    
    //Envoie des indices en memoire video
    glGenBuffers(1,&vboi_cylinder); PRINT_OPENGL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_cylinder);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);
}
static void calculate_cube_next_lane(){
    int next_lane = rand()%3;
    if((counter_time)%125 == 1){
        if (next_lane == 0 )
            cube_lane = left;
        else{
            if(next_lane==1)
                cube_lane = middle;
            else
                cube_lane = right;
        }
    }
}
static void renderBitmapString(float x, float y,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}


static void load_texture(){
    // Chargement d'une texture (seul les textures tga sont supportes)
    Image  *image = image_load_tga("texture.tga");

    if (image) //verification que l'image est bien chargee
    {
        
        //Creation d'un identifiant pour la texture
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &texture_id);
        
        //Selection de la texture courante a partir de son identifiant
        glBindTexture(GL_TEXTURE_2D, texture_id);
        
        //Parametres de la texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        //Envoie de l'image en memoire video
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
        
        delete image;
    }
    else
    {
        std::cerr<<"Erreur chargement de l'image, etes-vous dans le bon repertoire?"<<std::endl;
        abort();
    }
    glUniform1i (get_uni_loc(shader_program_id, "texture"), 0); PRINT_OPENGL_ERROR();

}
static void display_road(){
    //Placement du vbo courant (ici il n'y a qu'un seul vbo: optionnel)
    glBindBuffer(GL_ARRAY_BUFFER,vbo_cylinder); PRINT_OPENGL_ERROR();
    
    //Placement du pointeur pour les positions dans le buffer de donnees
    glEnableClientState(GL_VERTEX_ARRAY); PRINT_OPENGL_ERROR();
    glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0); PRINT_OPENGL_ERROR();
    
    //Placement du pointeur pour les normales dans le buffer de donnees
    glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();
    glNormalPointer(GL_FLOAT, sizeof(vertex_opengl),buffer_offset(sizeof(vec3))); PRINT_OPENGL_ERROR();
    
    //Placement du pointeur pour les couleurs dans le buffer de donnees
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_FLOAT,sizeof(vertex_opengl),buffer_offset(2*sizeof(vec3)));
    
    //Placement du pointeur pour les coordonnees de textures dans le buffer de donnees
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));  PRINT_OPENGL_ERROR();
    
    //Placement du vbo d'indice courant (ici il n'y a qu'un seul vboi: optionnel)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_cylinder);
    
    /**
     *
     *      Affichage de la Route
     *
     */
    
    angle = vec3(speed,0.0,0.0);
    object_id = 0;
    deplacement = vec3(0.0,0.0,0.0);
    
    glUniform1i(get_uni_loc(shader_program_id, "object_id"), object_id);PRINT_OPENGL_ERROR();
    
    glUniform1f(get_uni_loc(shader_program_id, "angle_x"), angle.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_y"), angle.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_z"), angle.z);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_x"), deplacement.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_y"), deplacement.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_z"), deplacement.z);PRINT_OPENGL_ERROR();
    glDrawElements(GL_TRIANGLES, /*2*(128-1)*(128-1)*3*/128*3, GL_UNSIGNED_INT, 0); PRINT_OPENGL_ERROR();
}
static void display_background(){
    
    
    //Placement du vbo courant (ici il n'y a qu'un seul vbo: optionnel)
    glBindBuffer(GL_ARRAY_BUFFER,vbo_background); PRINT_OPENGL_ERROR();
    
    //Placement du pointeur pour les positions dans le buffer de donnees
    glEnableClientState(GL_VERTEX_ARRAY); PRINT_OPENGL_ERROR();
    glVertexPointer(3, GL_FLOAT, sizeof(vertex_opengl), 0); PRINT_OPENGL_ERROR();
    
    //Placement du pointeur pour les normales dans le buffer de donnees
    glEnableClientState(GL_NORMAL_ARRAY); PRINT_OPENGL_ERROR();
    glNormalPointer(GL_FLOAT, sizeof(vertex_opengl),buffer_offset(sizeof(vec3))); PRINT_OPENGL_ERROR();
    
    //Placement du pointeur pour les couleurs dans le buffer de donnees
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_FLOAT,sizeof(vertex_opengl),buffer_offset(2*sizeof(vec3)));
    
    //Placement du pointeur pour les coordonnees de textures dans le buffer de donnees
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_opengl), buffer_offset(3*sizeof(vec3)));  PRINT_OPENGL_ERROR();
    
    //Placement du vbo d'indice courant (ici il n'y a qu'un seul vboi: optionnel)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi_background);
    
    angle = vec3(-0.422,0.0,0.0);
    deplacement=vec3(-2.5,0.20,-1.0);
    
    glUniform1i(get_uni_loc(shader_program_id, "object_id"), object_id);PRINT_OPENGL_ERROR();
    
    glUniform1f(get_uni_loc(shader_program_id, "angle_x"), angle.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_y"), angle.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_z"), angle.z);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_x"), deplacement.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_y"), deplacement.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_z"), deplacement.z);PRINT_OPENGL_ERROR();
    glColor3ub(255,255,255);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);PRINT_OPENGL_ERROR();
}
static void display_ship(){
        deplacement = vec3(0,1.2+0.02*cos(counter_time/10.0),0.0);
        angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);


        if(ship_previous_lane == right){
            if((counter_time_buff+5)>counter_time){
                deplacement = vec3(-0.25 + 0.05*(counter_time-counter_time_buff+1.0), 1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);
            }
            else{
                deplacement = vec3(0,1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);

            }
        }
        if(ship_previous_lane == left){
            if((counter_time_buff+5)>counter_time){
                deplacement = vec3(0.25 - 0.05*(counter_time-counter_time_buff+1.0), 1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);

            }
            else{
                deplacement = vec3(0,1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);

            }

        }
        if(ship_lane == left){
            if((counter_time_buff+5)>counter_time){
               deplacement = vec3(0.05*(counter_time-counter_time_buff+1.0),1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);

            }
            else
            {
                deplacement = vec3(0.25,1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);

            }
            

        }
        if(ship_lane == right){
            if((counter_time_buff+5)>counter_time)
            {
                deplacement = vec3(-0.05*(counter_time-counter_time_buff+1.0),1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);

            }
            else
            {
                deplacement = vec3(-0.25,1.2+0.02*cos(counter_time/10.0),0.0);
                angle = vec3(0.0,3.14+0.01*cos(counter_time/4.0),0.0);

            }

        }

  //  }

    object_id =2;
    glUniform1i(get_uni_loc(shader_program_id, "object_id"), object_id);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_x"), angle.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_y"), angle.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_z"), angle.z);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_x"), deplacement.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_y"), deplacement.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_z"), deplacement.z);PRINT_OPENGL_ERROR();
    
    glmDraw(ship,GLM_SMOOTH | GLM_MATERIAL);
}
static void display_cube(){
    /**
     *
     *      Affichage du cube
     *
     */
    position_cube = 1.57+speed;
    calculate_cube_next_lane();
    
    
    if(cube_lane == left){
        deplacement=vec3(-0.25,0.1,0.0);
        angle = vec3(position_cube,0.0,0.0);

    }
    if(cube_lane ==middle){
        deplacement=vec3(0.0,0.1,0.0);
        angle = vec3(position_cube,counter_time/5.0,0.0);
    }
    if(cube_lane == right){
        deplacement=vec3(0.25,0.1,0.0);
        angle = vec3(position_cube,0.0,0.0);


    }
    object_id =1;
    glUniform1i(get_uni_loc(shader_program_id, "object_id"), object_id);PRINT_OPENGL_ERROR();
    
    glUniform1f(get_uni_loc(shader_program_id, "angle_x"), angle.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_y"), angle.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_z"), angle.z);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_x"), deplacement.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_y"), deplacement.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_z"), deplacement.z);PRINT_OPENGL_ERROR();
    glColor3ub(255,255,255);
    glmDraw(cube, GLM_SMOOTH);
}
static void collision(){
    if((counter_time-seuil)%125 == 1){
        counter_seuil ++;
        if(counter_seuil ==8){
            seuil = seuil + 5;
            counter_seuil = 0;
        }
        if(ship_lane == cube_lane){
            nbvie--;
            glClearColor(1.0, 1.0, 1.0, 1.0);

        }
        if (nbvie == 0){
            //FIN de partie
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(1.0, 1.0, 1.0, 1.0);
            //On se place devant la caméra
            deplacement = vec3(0.0,0.0,0.0);
            angle = vec3(0,0.0,0.0);
            object_id = 2;
            glUniform1i(get_uni_loc(shader_program_id, "object_id"), object_id);PRINT_OPENGL_ERROR();
            glUniform1f(get_uni_loc(shader_program_id, "angle_x"), angle.x);PRINT_OPENGL_ERROR();
            glUniform1f(get_uni_loc(shader_program_id, "angle_y"), angle.y);PRINT_OPENGL_ERROR();
            glUniform1f(get_uni_loc(shader_program_id, "angle_z"), angle.z);PRINT_OPENGL_ERROR();
            glUniform1f(get_uni_loc(shader_program_id, "deplacement_x"), deplacement.x);PRINT_OPENGL_ERROR();
            glUniform1f(get_uni_loc(shader_program_id, "deplacement_y"), deplacement.y);PRINT_OPENGL_ERROR();
            glUniform1f(get_uni_loc(shader_program_id, "deplacement_z"), deplacement.z);PRINT_OPENGL_ERROR();
            
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, w, 0, h);
            glScalef(1, -1, 1);
            glTranslatef(0, -h, 0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glColor3d(0, 1, 0);
            glLoadIdentity();
            renderBitmapString(300,300,"GAME OVER");
            renderBitmapString(301,300,"GAME OVER");

            renderBitmapString(350,400,"score final:");
            renderBitmapString(400,425,  score);
            renderBitmapString(350,475,"r: restart, q : quitter");

            fin=1;
            
            
            
        }
    }
}
static void speed_up(){
    
        speed =  (counter_time/20.0);


}


static void HUD()
{
    //On se place devant la caméra
    deplacement = vec3(0.0,0.0,0.0);
    //on bloque la rotation
    angle = vec3(0,0.0,0.0);
    //On selectionne le texte dans le shader
    object_id = 2;
    glUniform1i(get_uni_loc(shader_program_id, "object_id"), object_id);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_x"), angle.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_y"), angle.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "angle_z"), angle.z);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_x"), deplacement.x);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_y"), deplacement.y);PRINT_OPENGL_ERROR();
    glUniform1f(get_uni_loc(shader_program_id, "deplacement_z"), deplacement.z);PRINT_OPENGL_ERROR();
    
    //On modifie les matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glScalef(1, -1, 1);
    glTranslatef(0, -h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(0, 1, 0);
    //On affiche du texte
    renderBitmapString(600,20,"Nombre de vies :");
    sprintf(vie,"%d",nbvie);
    renderBitmapString(760,20,  vie);
    renderBitmapString(20,20,"score :");
    renderBitmapString(100,20,  score);
    renderBitmapString(20,40,"Pause : p");
    renderBitmapString(20,60,  "? : e");
    renderBitmapString(20,80,  "débug : i/o");
    renderBitmapString(20,100,  "quitter : q");

}




//Fonction d'affichage
static void display_callback()
{
    
    if(!fin)
    {
    HUD();
    
    //mise en place de la matrice de projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(100.0,1.6,0.01,60.0); //une matrice de perspective (affiche profondeur z entre [0.1,20])
    //mise en place de la matrice "monde"
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f,-1.33f,-1.0f); //deplacement de la camera vers l'arriere
    glRotatef(24.0,1.0f,0.0f,0.0f); //rotation suivant un axe
    glRotatef(0.0f,0.0f,2.0f,0.0f); //rotation suivant l'autre axe


    
    //effacement des couleurs du fond d'ecran
    glClearColor(0.0, 0.0, 0.0, 1.0);
    display_road();
    display_background();
    display_ship();
    if (!debug) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    display_cube();
    if (!debug) glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    collision();
    speed_up();


   
    //Changement de buffer d'affichage pour eviter un effet de scintillement
    glutSwapBuffers();
    PRINT_OPENGL_ERROR(); //recherche d'erreurs opengl
    }
}

/*****************************************************************************\
 * init                                                                      *
\*****************************************************************************/
static void init()
{
    
    generate_cylinder();
    generate_background();
    w=800;
    h=500;
    // Chargement du shader
    
    std::string vertex_shader ="shader.vert";
    std::string fragment_shader ="shader.frag";
    shader_program_id = read_shader(vertex_shader, fragment_shader);
    PRINT_OPENGL_ERROR();

   load_texture();
    
    // Chargement d'une texture (seul les textures tga sont supportes)

    //activation de la gestion de la profondeur
    glEnable(GL_DEPTH_TEST ); PRINT_OPENGL_ERROR();

}

/*****************************************************************************\
 * keyboard_callback                                                         *
\*****************************************************************************/
static void keyboard_callback(unsigned char key, int, int)
{
    //quitte le programme si on appuis sur les touches 'q', 'Q', ou 'echap'
    switch (key)
    {
        case 'o':
        case 'O':  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ), debug=1;
            break;
        case 'i':
        case 'I': glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ), debug=0;
            break;
        case 'p':
        case 'P': pause=!pause;
            break;
        case 'r':
        case 'R':
            if(fin)
            {
                nbvie=3;
                counter_time=0;
                fin=0;
                position_vaisseau = 0;
                counter_time_buff = 50;
            }
            
            break;
        case 'e':
        case 'E':
            nbvie++;
            break;
        case 'q':
        case 'Q':
        case 27:
        exit(0);
    }
}

/*****************************************************************************\
 * special_callback                                                          *
\*****************************************************************************/
static void special_callback(int key, int,int)
{
    if(!pause)
    {
        switch (key)
        {
            case GLUT_KEY_UP:
          
                break;
          
            case GLUT_KEY_DOWN:
          
                break;
          
            case GLUT_KEY_LEFT:
                if(ship_lane == middle || ship_lane == right){
                    counter_time_buff=counter_time;
                    ship_previous_lane = ship_lane;
                    if(ship_lane==middle)
                        ship_lane = left;
                    else
                        ship_lane = middle;
                }
                break;
            case GLUT_KEY_RIGHT:
                if(ship_lane == middle || ship_lane == left){
                    counter_time_buff=counter_time;
                    ship_previous_lane = ship_lane;
                    if(ship_lane==middle)
                        ship_lane= right;
                    else
                        ship_lane = middle;
                }
                break;
        }
    }

  //reactualisation de l'affichage
  glutPostRedisplay();
}

/*****************************************************************************\
 * timer_callback                                                            *
\*****************************************************************************/
static void timer_callback(int)
{
    //demande de rappel de cette fonction dans 25ms
    glutTimerFunc(25, timer_callback, 0);

    if(!pause)
    {
    counter_time++;
    int time = (int)counter_time;
    sprintf(score, "%2d", time);
    //reactualisation de l'affichage
    glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    char objet[20];
    //**********************************************//
    //Lancement des fonctions principales de GLUT
    //**********************************************//

    //initialisation
    glutInit(&argc, argv);

    //Mode d'affichage (couleur, gestion de profondeur, ...)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    //Taille de la fenetre a l'ouverture
    glutInitWindowSize(800, 500);

    //Titre de la fenetre
    glutCreateWindow("OpenGL");

    //Fonction de la boucle d'affichage
    glutDisplayFunc(display_callback);

    //Fonction de gestion du clavier
    glutKeyboardFunc(keyboard_callback);

    //Fonction des touches speciales du clavier (fleches directionnelles)
    glutSpecialFunc(special_callback);

    //Fonction d'appel d'affichage en chaine
    glutTimerFunc(25, timer_callback, 0);


    //Notre fonction d'initialisation des donnees et chargement des shaders
    init();
    

    strcpy(objet,"vaisseau.obj");
    ship = glmReadOBJ(objet);
    strcpy(objet,"cube.obj");
    cube = glmReadOBJ(objet);
    

    //Lancement de la boucle (infinie) d'affichage de la fenetre
    glutMainLoop();

    //Plus rien n'est execute apres cela

    return 0;
}

