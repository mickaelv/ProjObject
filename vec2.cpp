
#include "vec2.hpp"
#include <cmath>
#include <cassert>
#include <iostream>




vec2::vec2()
    :x(0.0f),y(0.0f)
{}

vec2::vec2(float x_param,float y_param)
    :x(x_param),y(y_param)
{}

vec2& vec2::operator+=(const vec2& v)
{
    x+=v.x;
    y+=v.y;
    return *this;
}

vec2& vec2::operator-=(const vec2& v)
{
    x-=v.x;
    y-=v.y;
    return *this;
}

vec2& vec2::operator*=(float s)
{
    x*=s; y*=s;
    return *this;
}

vec2& vec2::operator/=(float s)
{
    assert(fabs(s)>10e-6);
    x/=s; y/=s;
    return *this;
}

void print_screen(const vec2& v)
{
    std::cout<<v.x<<" "<<v.y<<std::endl;
}

float norm(const vec2& v)
{
    return std::sqrt(v.x*v.x+v.y*v.y);
}

float dot(const vec2& v0,const vec2& v1)
{
    return v0.x*v1.x+v0.y*v1.y;
}

vec2 normalize(vec2& v)
{
    vec2 temp=v;
    temp/=norm(v);
    return temp;
}

vec2 operator+(const vec2& v0,const vec2& v1)
{
    vec2 temp=v0;
    temp+=v1;
    return temp;
}

vec2 operator-(const vec2& v0,const vec2& v1)
{
    vec2 temp=v0;
    temp-=v1;
    return temp;
}

vec2 operator*(const vec2& v0,float s)
{
    vec2 temp=v0;
    temp*=s;
    return temp;
}

vec2 operator*(float s,const vec2& v0)
{
    return v0*s;
}

vec2 operator/(const vec2& v0,float s)
{
    vec2 temp=v0;temp/=s;return temp;
}


