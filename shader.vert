#version 120


varying vec3 normal;
varying vec4 vertex_3d_original;
varying vec4 vertex_3d_modelview;

uniform int time;

uniform float angle_x;
uniform float angle_y;
uniform float angle_z;

uniform float deplacement_x;
uniform float deplacement_y;
uniform float deplacement_z;

mat4 rotation_x(float angle){
    mat4 rotation;
    rotation[0]=vec4(1.0,0.0,0.0,0.0);
    rotation[1]=vec4(0.0,cos(angle),sin(angle),0.0);
    rotation[2]=vec4(0.0,-sin(angle),cos(angle),0.0);
    rotation[3]=vec4(0.0,0.0,0.0,1.0);
    return rotation;
}
mat4 rotation_y(float angle){
    mat4 rotation;
    rotation[0]=vec4(cos(angle),0.0,sin(angle),0.0);
    rotation[1]=vec4(0.0,1.0,0.0,0.0);
    rotation[2]=vec4(-sin(angle),0.0,cos(angle),0.0);
    rotation[3]=vec4(0.0,0.0,0.0,1.0);
    return rotation;
}

mat4 rotation_z(float angle){
    mat4 rotation;
    rotation[0]=vec4(cos(angle),sin(angle),0.0,0.0);
    rotation[1]=vec4(-sin(angle),cos(angle),0.0,0.0);
    rotation[2]=vec4(0.0,0.0,1.0,0.0);
    rotation[3]=vec4(0.0,0.0,0.0,1.0);
    return rotation;
}


mat4 translation_x(float x, float y , float z){
    mat4 translation;
    translation[0]=vec4(1.0,0.0,0.0,0.0);
    translation[1]=vec4(0.0,1.0,0.0,0.0);
    translation[2]=vec4(0.0,0.0,1.0,0.0);
    translation[3]=vec4(x,y,z,1.0);
    
    return translation;
}



void main (void)
{
  

  
    mat4 rotation_x = rotation_x(angle_x);
    mat4 rotation_y = rotation_y(angle_y);
    mat4 rotation_z = rotation_z(angle_z);
    
    mat4 translation = translation_x(deplacement_x,deplacement_y,deplacement_z);
  
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * rotation_x * rotation_y  *rotation_z*  translation * gl_Vertex;
    normal = gl_NormalMatrix * normalize(gl_Normal);
    vertex_3d_original=gl_Vertex;
    vertex_3d_modelview=gl_ModelViewMatrix*gl_Vertex;
    gl_TexCoord[0]=gl_MultiTexCoord0;
}

