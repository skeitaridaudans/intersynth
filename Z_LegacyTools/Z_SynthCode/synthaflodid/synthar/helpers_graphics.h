#ifndef HELPERS_GRAPHICS_H_8736483745
#define HELPERS_GRAPHICS_H_8736483745


/**
 * Sample multi-touch program that displays a square where a cursor
 * clicks, with a different color for each cursor.
 *
 * Copyright (C) 2012  Sylvain Beucler
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <GL/gl.h>

#include <vector>
#include <list>
#include <math.h>

#define TAU 6.28318530717958647692

struct Coordinate{
    Coordinate(){
        this->x = 0;
        this->y = 0;
    }
    Coordinate(float x, float y){
        this->x = x;
        this->y = y;
    }
    float dot(const Coordinate &other) const{
        return this->x * other.x + this->y * other.y;
    }
    float length() const{
        return sqrt(dot(*this));
    }
    void normalize(){
        float l = length();
        this->x = this->x / l;
        this->y = this->y / l;
    }
    Coordinate parallel_part(const Coordinate &normal){
        return this->dot(normal) / normal.dot(normal) * normal;
    }
    void bounce_off(const Coordinate &normal, bool skip_backface_bounce = true){
        if(!skip_backface_bounce || this->dot(normal) < 0.0){
            *this = (*this) - 2 * this->dot(normal) / normal.dot(normal) * normal;
        }
    }
    Coordinate normalized() const{
        float l = length();
        return Coordinate(this->x / l, this->y / l);
    }
    Coordinate operator+(const Coordinate &other) const{
        return Coordinate(this->x + other.x, this->y + other.y);
    }
    Coordinate operator-(const Coordinate &other) const{
        return Coordinate(this->x - other.x, this->y - other.y);
    }
    Coordinate operator*(const float &s){
        return Coordinate(s * this->x, s * this->y);
    }
    friend Coordinate operator*(const float &s, const Coordinate &coord){
        return Coordinate(s * coord.x, s * coord.y);
    }
    Coordinate operator/(const float &s){
        return Coordinate(this->x / s, this->y / s);
    }
    float x;
    float y;
};

struct Color{
    Color(){
        r = 0;
        g = 0;
        b = 0;
    }
    Color(float r, float g, float b){
        this->r = r;
        this->g = g;
        this->b = b;
    }
    float r;
    float g;
    float b;
};


struct Area{
    Area(){}
    Area(float x, float y, float w, float h, float c){
        center.x = x;
        center.y = y;
        width = w;
        height = h;
        color = Color(c, c, c);
    }
    Area(float x, float y, float w, float h, float r, float g, float b){
        center.x = x;
        center.y = y;
        width = w;
        height = h;
        color = Color(r, g, b);
    }
    Coordinate center;
    float width;
    float height;
    Color color;

    bool is_inside(float x, float y){
        return center.x - (width / 2.0) < x && x < center.x + (width / 2.0)
            && center.y - (height / 2.0) < y && y < center.y + (height / 2.0);
    }
};

struct Circle{
    Circle(){}
    Circle(float x, float y, float r, float c){
        center.x = x;
        center.y = y;
        radius = r;
        color = Color(c, c, c);
    }
    Circle(float x, float y, float r, float red, float green, float blue){
        center.x = x;
        center.y = y;
        radius = r;
        color = Color(red, green, blue);
    }
    Coordinate center;
    float radius;
    Color color;

    bool is_inside(float x, float y){
        float vec_x = x - center.x;
        float vec_y = y - center.y;
        return (vec_x * vec_x + vec_y * vec_y) < (radius * radius);
    }

    bool intersects(Circle *other, Coordinate &collision_normal, Coordinate &collision_point){
        float combined_radius = this->radius + other->radius;
        if(abs(center.x - other->center.x) > combined_radius || abs(center.y - other->center.y) > combined_radius){
            return false;
        }
        collision_normal = this->center - other->center;
        if((collision_normal.x * collision_normal.x + collision_normal.y * collision_normal.y) < combined_radius * combined_radius){
            collision_normal.normalize();
            collision_point = other->center + other->radius * collision_normal;
            return true;
        }
        return false;
    }
    bool intersects(const Coordinate &direction, Area *area, Coordinate &collision_normal, Coordinate &collision_point){
        if(abs(center.x - area->center.x) > (radius + area->width)
        || abs(center.y - area->center.y) > (radius + area->height)){
            return false;
        }
        if(direction.y < 0 && area->is_inside(center.x, center.y - radius)){
            collision_normal.x = 0;
            collision_normal.y = 1;
            collision_point.x = center.x;
            collision_point.y = area->center.y + area->height / 2;
            return true;
        }
        if(direction.x < 0 && area->is_inside(center.x - radius, center.y)){
            collision_normal.x = 1;
            collision_normal.y = 0;
            collision_point.x = area->center.x + area->width / 2;
            collision_point.y = center.y;
            return true;
        }
        if(direction.y > 0 && area->is_inside(center.x, center.y + radius)){
            collision_normal.x = 0;
            collision_normal.y = -1;
            collision_point.x = center.x;
            collision_point.y = area->center.y - area->height / 2;
            return true;
        }
        if(direction.x > 0 && area->is_inside(center.x + radius, center.y)){
            collision_normal.x = -1;
            collision_normal.y = 0;
            collision_point.x = area->center.x - area->width / 2;
            collision_point.y = center.y;
            return true;
        }
        if((direction.x < 0 || direction.y < 0) && is_inside(area->center.x + area->width / 2, area->center.y + area->height / 2)){
            collision_normal.x = this->center.x - (area->center.x + area->width / 2);
            collision_normal.y = this->center.y - (area->center.y + area->height / 2);
            collision_normal.normalize();
            collision_point.x = area->center.x + area->width / 2;
            collision_point.y = area->center.y + area->height / 2;
            return true;
        }
        if((direction.x < 0 || direction.y > 0) && is_inside(area->center.x + area->width / 2, area->center.y - area->height / 2)){
            collision_normal.x = this->center.x - (area->center.x + area->width / 2);
            collision_normal.y = this->center.y - (area->center.y - area->height / 2);
            collision_normal.normalize();
            collision_point.x = area->center.x + area->width / 2;
            collision_point.y = area->center.y - area->height / 2;
            return true;
        }
        if((direction.x > 0 || direction.y > 0) && is_inside(area->center.x - area->width / 2, area->center.y - area->height / 2)){
            collision_normal.x = this->center.x - (area->center.x - area->width / 2);
            collision_normal.y = this->center.y - (area->center.y - area->height / 2);
            collision_normal.normalize();
            collision_point.x = area->center.x - area->width / 2;
            collision_point.y = area->center.y - area->height / 2;
            return true;
        }
        if((direction.x > 0 || direction.y < 0) && is_inside(area->center.x - area->width / 2, area->center.y + area->height / 2)){
            collision_normal.x = this->center.x - (area->center.x - area->width / 2);
            collision_normal.y = this->center.y - (area->center.y + area->height / 2);
            collision_normal.normalize();
            collision_point.x = area->center.x - area->width / 2;
            collision_point.y = area->center.y + area->height / 2;
            return true;
        }
        return false;
    }
};


void draw_rectangle(float center_x, float center_y, float width, float height, Color color, bool outline = false);
void draw_area(Area &area);
void draw_area(Area &area, float rotation);
void draw_areas(std::vector<Area *> &areas);
void draw_areas(std::list<Area *> &areas);
void draw_line_strip(float *vertices, int vertex_count, float color, bool loop = false);
void draw_line(float x1, float y1, float x2, float y2, float color);
void draw_circle(float x, float y, float r, Color color, int slices = 12, bool outline = true);
void draw_circle(Circle &circle, int slices = 12, bool outline = true);
void draw_ruler(float max_ms);

struct Cursor {
    Cursor() {
        this->on = false;
        this->area.center.x = -1;
        this->area.center.y = -1;
        this->area.width = 30.0;
        this->area.height = 30.0;
        this->area.color = Color(1.0, 1.0, 1.0);
    }
    Cursor(char on, float x, float y) : Cursor(){
        this->on = on;
        this->area.center.x = x;
        this->area.center.y = y;
    }
    char on;
    Area area;

    void setX(float x){
        this->area.center.x = x;
    }

    float getX(){
        return this->area.center.x;
    }

    void setY(float y){
        this->area.center.y = y;
    }

    float getY(){
        return this->area.center.y;
    }
};


#endif //HELPERS_GRAPHICS_H_8736483745