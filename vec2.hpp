
#pragma once

#ifndef VEC2_HPP
#define VEC2_HPP



struct vec2
{
    float x;
    float y;

    vec2();
    vec2(float x_param,float y_param);

    vec2& operator+=(const vec2& v);
    vec2& operator-=(const vec2& v);
    vec2& operator*=(float s);
    vec2& operator/=(float s);
};
void print_screen(const vec2& v);
float norm(const vec2& v);
float dot(const vec2& v0,const vec2& v1);
vec2 normalize(vec2& v);
vec2 operator+(const vec2& v0,const vec2& v1);
vec2 operator-(const vec2& v0,const vec2& v1);
vec2 operator*(const vec2& v0,float s);
vec2 operator*(float s,const vec2& v0);
vec2 operator/(const vec2& v0,float s);




#endif
