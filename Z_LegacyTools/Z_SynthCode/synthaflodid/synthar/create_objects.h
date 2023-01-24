#include <GL/freeglut.h>

#include "helpers_graphics.h"

#define TONE_BALL_SIZE 20

enum ObjectMode{
    TONE_BRICK = 0,
    TONE_SPHERE,
    BLACK_HOLE,
    BEND_SPHERE,
    MODULATION_FIELD
};

struct ToneBrick : public Area{
    ToneBrick(float x, float y, float w, float h, float r, float g, float b, int note) : Area(x, y, w, h, r, g, b){
        original_note = midi_note = note;
        touch_id = -1;
        invalid = false;
    };

    int get_note(){
        return midi_note;
    }
    bool touch(int id, float x, float y){
        if(touch_id == -1 && is_inside(x, y)){
            touch_id = id;
            touch_offset.x = x - center.x;
            touch_offset.y = y - center.y;
            return true;
        }
        return false;
    }
    bool touch_motion(int id, float x, float y){
        if(id == touch_id){
            center.x = x - touch_offset.x;
            center.y = y - touch_offset.y;
            return true;
        }
        return false;
    }
    bool untouch(int id, int area_start, int area_width){
        if(touch_id == id){
            touch_id = -1;
            if(center.x < (area_start - width / 2) || center.x > (area_start + area_width + width / 2)){
                invalid = true;
            }
            return true;
        }
        return false;
    }
    int midi_note;
    int original_note;
    int touch_id;
    Coordinate touch_offset;
    bool invalid;
};

int slices_from_radius(float radius){
    return std::max(12, 12 * (int)(radius / 25));
}

struct ToneBall : public Circle{
    ToneBall(float x, float y, float r, float red, float green, float blue, int note, Coordinate v) : Circle(x, y, r, red, green, blue){
        original_note = midi_note = note;
        velocity = v;
        touch_id = -1;
        invalid = false;
    }
    void update_auto(float elapsed_time){
        center.x += velocity.x * elapsed_time;
        center.y += velocity.y * elapsed_time;
    }
    void draw(bool outline = true){
        draw_circle(*this, slices_from_radius(radius), outline);
    }
    bool touch(int id, float x, float y){
        if(touch_id == -1 && is_inside(x, y)){
            touch_id = id;
            touch_offset.x = x - center.x;
            touch_offset.y = y - center.y;
            previous_time = glutGet(GLUT_ELAPSED_TIME);
            return true;
        }
        return false;
    }
    bool touch_motion(int id, float x, float y){
        if(id == touch_id){
            int new_time = glutGet(GLUT_ELAPSED_TIME);
            float delta_time = float(new_time - previous_time) / 1000.0;
            if(delta_time > 0.02){
                previous_time = new_time;
                Coordinate change = Coordinate(x - touch_offset.x - center.x, y - touch_offset.y - center.y);
                velocity = change / delta_time;
                float l = velocity.length();
                if(l > 200.0){
                    velocity = 200.0 * velocity / l;
                }
            }

            center.x = x - touch_offset.x;
            center.y = y - touch_offset.y;
            return true;
        }
        return false;
    }
    bool untouch(int id, float area_start, float area_width){
        if(touch_id == id){
            touch_id = -1;
            if(center.x < (area_start - radius) || center.x > (area_start + area_width + radius)){
                invalid = true;
            }
            else{
                velocity.x = 0;
                velocity.y = 0;
            }
            return true;
        }
        return false;
    }
    int midi_note;
    int original_note;
    int touch_id;
    Coordinate touch_offset;
    Coordinate velocity;
    bool invalid;
    int previous_time;
};

struct DrawingObject{
    DrawingObject(){
        touch_id = -1;
        mode = TONE_BRICK;
    }
    void *make_object(int midi_note){
        if(touch_id == -1){
            return NULL;
        }
        touch_id = -1;
        switch(mode){
        case TONE_BRICK:
            return new ToneBrick((far_corner.x + first_corner.x) / 2.0, (far_corner.y + first_corner.y) / 2.0,
                        abs(far_corner.x - first_corner.x), abs(far_corner.y - first_corner.y),
                        next_color.r, next_color.g, next_color.b, midi_note);
            break;
        case TONE_SPHERE:
            return new ToneBall(first_corner.x, first_corner.y, (far_corner - first_corner).length(),
                                next_color.r, next_color.g, next_color.b, midi_note, Coordinate(0 ,0));
            break;
        case BLACK_HOLE:
            return new ToneBall(first_corner.x, first_corner.y, (far_corner - first_corner).length(),
                                0.3, 0.3, 0.3, midi_note, Coordinate(0 ,0));
            break;
        case BEND_SPHERE:
            return NULL;
            break;
        case MODULATION_FIELD:
            return NULL;
            break;
        default:
            return NULL;
        }
    }
    void set_mode(ObjectMode new_mode){
        if(touch_id == -1){
            mode = new_mode;
        }
    }
    void draw(){
        if(touch_id == -1){
            return;
        }
        switch(mode){
        case TONE_BRICK:
            draw_rectangle((far_corner.x + first_corner.x) / 2.0, (far_corner.y + first_corner.y) / 2.0,
                        abs(far_corner.x - first_corner.x), abs(far_corner.y - first_corner.y),
                        next_color, true);
            break;
        case TONE_SPHERE:{
            float rad = (far_corner - first_corner).length();
            draw_circle(first_corner.x, first_corner.y, rad, next_color, slices_from_radius(rad));
            }
            break;
        case BLACK_HOLE:{
            float rad = (far_corner - first_corner).length();
            draw_circle(first_corner.x, first_corner.y, rad, Color(0.5, 0.5, 0.5), slices_from_radius(rad));
            }
            break;
        case BEND_SPHERE:
            break;
        case MODULATION_FIELD:
            break;
        default:
            break;
        }
    }
    bool touch(int id, float x, float y){
        if(touch_id == -1){
            touch_id = id;
            first_corner.x = far_corner.x = x;
            first_corner.y = far_corner.y = y;
            next_color = Color((float)(4 + ((int)rand()) % 7) / 10.0, (float)(4 + ((int)rand()) % 7) / 10.0, (float)(4 + ((int)rand()) % 7) / 10.0);
            return true;
        }
        return false;
    }
    bool touch_motion(int id, float x, float y){
        if(id == touch_id){
            far_corner.x = x;
            far_corner.y = y;
            return true;
        }
        return false;
    }
    bool untouch(int id){
        if(touch_id == id){
            touch_id = -1;
            return true;
        }
        return false;
    }
    Coordinate first_corner;
    Coordinate far_corner;
    int touch_id;
    Color next_color;
    ObjectMode mode;
};
