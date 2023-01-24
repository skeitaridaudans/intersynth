
#include "helpers_graphics.h"

#include <map>

static float square[] = {
        -.5, -.5,
         .5, -.5,
         .5,  .5,
        -.5,  .5,
    };

void draw_rectangle(float center_x, float center_y, float width, float height, Color color, bool outline){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, square);

    glPushMatrix();
    glTranslatef(center_x, center_y, 0);
    glScalef(width, height, 1);
    
    glColor4f(color.r, color.g, color.b, 1);
    if(outline){
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }
    else{
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);

}

void draw_area(Area &area){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, square);

    glPushMatrix();
    glTranslatef(area.center.x, area.center.y, 0);
    glScalef(area.width, area.height, 1);
    
    glColor4f(area.color.r,area.color.g,area.color.b,1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);

}

void draw_area(Area &area, float rotation){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, square);

    glPushMatrix();
    glTranslatef(area.center.x, area.center.y, 0);
    glRotatef(rotation, 0.0, 0.0, 1.0);
    glScalef(area.width, area.height, 1);
    
    glColor4f(area.color.r,area.color.g,area.color.b,1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);

}

void draw_areas(std::vector<Area *> &areas){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, square);

    for(Area *area : areas){
        
        glPushMatrix();
        glTranslatef(area->center.x, area->center.y, 0);
        glScalef(area->width, area->height, 1);
        
        glColor4f(area->color.r,area->color.g,area->color.b,1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();
    }

    glDisableClientState(GL_VERTEX_ARRAY);

}

void draw_areas(std::list<Area *> &areas){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, square);

    for(Area *area : areas){
        
        glPushMatrix();
        glTranslatef(area->center.x, area->center.y, 0);
        glScalef(area->width, area->height, 1);
        
        glColor4f(area->color.r,area->color.g,area->color.b,1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();
    }

    glDisableClientState(GL_VERTEX_ARRAY);

}

void draw_line_strip(float *vertices, int vertex_count, float color, bool loop){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);

    glColor4f(color, color, color, 1);
    if(loop){
        glDrawArrays(GL_LINE_LOOP, 0, vertex_count);
    }
    else{
        glDrawArrays(GL_LINE_STRIP, 0, vertex_count);
    }

    glDisableClientState(GL_VERTEX_ARRAY);

}

std::map<int, float*> circle_vertices;
// float circle_vertices[24];
// bool circle_initialized = false;

void draw_circle(float x, float y, float r, Color color, int slices, bool outline){
    if(circle_vertices.find(slices) == circle_vertices.end()){
        float *cvert = new float[slices * 2];
        float angle = 0.0;
        for(int i = 0; i < slices * 2; i += 2){
            cvert[i] = cos(angle);
            cvert[i+1] = sin(angle);
            angle += TAU / (float)slices;

        }
        circle_vertices[slices] = cvert;
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, circle_vertices[slices]);

    glColor4f(color.r, color.g, color.b, 1);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(r, r, 1);
    if(outline){
        glDrawArrays(GL_LINE_LOOP, 0, slices);
    }
    else{
        glDrawArrays(GL_TRIANGLE_FAN, 0, slices);
    }
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
}

void draw_circle(Circle &circle, int slices, bool outline){
    draw_circle(circle.center.x, circle.center.y, circle.radius, circle.color, slices, outline);
}

float line_vertices[4];

void draw_line(float x1, float y1, float x2, float y2, float color){
    line_vertices[0] = x1;
    line_vertices[1] = y1;
    line_vertices[2] = x2;
    line_vertices[3] = y2;

    draw_line_strip(line_vertices, 2, color);
}


void draw_ruler(float max_ms){
    float max_graphics = max_ms * 10.0;
    float scaler = 0.1;
    while(max_graphics > 2500.0){
        max_graphics = max_graphics / 10.0;
        scaler = scaler * 10.0;
    }
    for(float f = 0.0; f < max_graphics; f += 10.0){
        if((int)f % 100 == 0){
            draw_line(scaler * f, 0.0, scaler * f, 0.25, 1.0);
        }
        else if((int)f % 50 == 0){
            draw_line(scaler * f, 0.0, scaler * f, 0.15, 1.0);
        }
        else{
            draw_line(scaler * f, 0.0, scaler * f, 0.1, 1.0);
        }
    }
}
