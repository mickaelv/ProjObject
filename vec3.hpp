
#pragma once

#ifndef VEC3_HPP
#define VEC3_HPP


struct vec3
{
    float x;
    float y;
    float z;

    vec3();
    vec3(float x_param,float y_param,float z_param);

    vec3& operator+=(const vec3& v);
    vec3& operator-=(const vec3& v);
    vec3& operator*=(float s);
    vec3& operator/=(float s);
};
void print_screen(const vec3& v);
float norm(const vec3& v);
float dot(const vec3& v0,const vec3& v1);
vec3 cross(const vec3& v0,const vec3& v1);
vec3 normalize(const vec3& v);
vec3 operator+(const vec3& v0,const vec3& v1);
vec3 operator-(const vec3& v0,const vec3& v1);
vec3 operator*(const vec3& v0,float s);
vec3 operator*(float s,const vec3& v0);
vec3 operator/(const vec3& v0,float s);






#endif
