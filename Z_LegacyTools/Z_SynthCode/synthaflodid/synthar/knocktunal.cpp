/** @file knocktunal.cpp
    @ingroup synthaflodid
    @brief Send midi data based on game events.
    @author Kári Halldórsson
*/

#include <iostream>
#include <stdio.h>
#include <math.h>

#include "helpers_graphics.h"
#include "helpers_midi.h"
#include "knocktunal_midi.h"
#include "create_objects.h"

#include <list>
#include <vector>
#include <unordered_map>

#include <random>

struct ToneCannon : public Area{
    ToneCannon(float x, float y, float w, float h, float r, float g, float b) : Area(x, y, w, h, r, g, b){
        rotation = 90.0;
        touch_distance = (w / 2.0) - (h / 2.0);
        circle_radius = h / 2.0 + 5.0;
        touch_id = -1;
        circle = Circle(this->center.x + cos(rotation * TAU / 360.0) * touch_distance, this->center.y + sin(rotation * TAU / 360.0) * touch_distance, circle_radius, 1.0, 1.0, 0.0);
    }
    void reset_sizes(float x, float y, float w, float h){
        center.x = x;
        // center.y = y;
        // width = w;
        // height = h;
        // touch_distance = (w / 2.0) - (h / 2.0);
        // circle_radius = h / 2.0 - 5.0;
        circle = Circle(this->center.x + cos(rotation * TAU / 360.0) * touch_distance, this->center.y + sin(rotation * TAU / 360.0) * touch_distance, circle_radius, 1.0, 1.0, 0.0);
    }
    void update(float elapsed_time){
        // rotation += 90.0 * elapsed_time;
        circle.center.x = this->center.x + cos(rotation * TAU / 360.0) * touch_distance;
        circle.center.y = this->center.y + sin(rotation * TAU / 360.0) * touch_distance;
    }
    bool will_fire(){
        return (touch_id != -1);
    }
    ToneBall *fire(int midi_note, float power = 100.0){
        if(touch_id == -1){
            return NULL;
        }
        ToneBall *tb = new ToneBall(this->center.x + cos(rotation * TAU / 360.0) * (width + height) / 2,
                            this->center.y + sin(rotation * TAU / 360.0) * (width + height) / 2,
                            TONE_BALL_SIZE, next_color.r, next_color.g, next_color.b, midi_note,
                            Coordinate(cos(rotation * TAU / 360.0) * power, sin(rotation * TAU / 360.0) * power));
        next_color = Color((float)(4 + ((int)rand()) % 7) / 10.0, (float)(4 + ((int)rand()) % 7) / 10.0, (float)(4 + ((int)rand()) % 7) / 10.0);
        return tb;
    }
    void draw(){
        draw_area(*this, rotation);
        draw_circle(circle);
        if(touch_id != -1){
            draw_circle(this->center.x + cos(rotation * TAU / 360.0) * (width + height) / 2, this->center.y + sin(rotation * TAU / 360.0) * (width + height) / 2, TONE_BALL_SIZE, next_color);
        }
    }
    void set_rotation(float x, float y){
        if(y < this->center.y){
            rotation = x < this->center.x ? 180.0 : 0.0;
        }
        else{
            rotation = atan((y - this->center.y) / (x - this->center.x)) * 360.0 / TAU;
            if(rotation < 0.0){
                rotation += 180.0;
            }
        }
    }
    bool touch(int touch_id, float x, float y){
        if(this->touch_id == -1){
            if(circle.is_inside(x, y)){
                this->touch_id = touch_id;
                set_rotation(x, y);
                next_color = Color((float)(4 + ((int)rand()) % 7) / 10.0, (float)(4 + ((int)rand()) % 7) / 10.0, (float)(4 + ((int)rand()) % 7) / 10.0);
                return true;
            }
        }
        return false;
    }
    bool touch_motion(int touch_id, float x, float y){
        if(touch_id == this->touch_id){
            set_rotation(x, y);
            return true;
        }
        return false;
    }
    bool untouch(int touch_id){
        if(touch_id == this->touch_id){
            this->touch_id = -1;
            return true;
        }
        return false;
    }
    float rotation;
    float touch_distance;
    float circle_radius;
    int touch_id;
    Circle circle;
    Color next_color;
};



int screen_width = 800;
int screen_height = 480;

#define EDGE_WIDTH 160


std::list<Area *> bricks;
std::vector<Area *> outside_edges;
std::list<Circle *> balls;
std::list<Circle *> spheres;
std::list<Circle *> holes;
std::list<NoteMessage> note_messages;
std::list<CollisionNote *> living_notes;
ToneCannon *cannon;

DrawingObject next_object;


std::vector<Area *> object_mode_buttons;

int init_opengl_glut(int &argc, char* argv[]);

int main(int argc, char* argv[])
{
    set_note_list(&note_messages);

    init_midi_in(midiCallback);
    init_midi_out();

    outside_edges.push_back(new Area(EDGE_WIDTH / 2.0, screen_height / 2, EDGE_WIDTH, screen_height, 0.7, 0.3, 0.2));
    outside_edges.push_back(new Area(screen_width - EDGE_WIDTH / 2.0, screen_height / 2, EDGE_WIDTH, screen_height, 0.7, 0.3, 0.2));
    outside_edges.push_back(new Area(screen_width / 2, screen_height + 100, screen_width, 220, 1.0, 1.0, 1.0));
    outside_edges.push_back(new Area(screen_width / 2, -100, screen_width, 220, 1.0, 1.0, 1.0));
    outside_edges.push_back(new Area(EDGE_WIDTH, screen_height / 2, 10, screen_height, 1.0, 1.0, 1.0));
    outside_edges.push_back(new Area(screen_width - EDGE_WIDTH, screen_height / 2, 10, screen_height, 1.0, 1.0, 1.0));

    object_mode_buttons.push_back(new Area(EDGE_WIDTH / 2, screen_height / 2 + 120, 100, 100, 0.0));
    object_mode_buttons.push_back(new Area(EDGE_WIDTH / 2, screen_height / 2, 100, 100, 0.0));
    object_mode_buttons.push_back(new Area(EDGE_WIDTH / 2, screen_height / 2 - 120, 100, 100, 0.0));
    object_mode_buttons.push_back(new Area(screen_width - EDGE_WIDTH / 2, screen_height / 2 + 120, 100, 100, 0.0));
    object_mode_buttons.push_back(new Area(screen_width - EDGE_WIDTH / 2, screen_height / 2, 100, 100, 0.0));
    object_mode_buttons.push_back(new Area(screen_width - EDGE_WIDTH / 2, screen_height / 2 - 120, 100, 100, 0.0));

    cannon = new ToneCannon(400, 20, 200, 40, 0.7, 0.3, 0.2);

    // bricks.push_back(new ToneBrick(60, 120, 50, 100, 1.0, 1.0, 1.0, 38));

    // balls.push_back(new ToneBall(200, 200, TONE_BALL_SIZE, 1.0, 0.4, 0.4, 62, Coordinate(400, 80)));

    init_opengl_glut(argc, argv);

    uninit_midi_out();
    // printf("Test finished.\n");
}





#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/freeglut.h>

bool normal_mouse_down = false;
const static int NORMAL_MOUSE_CURSOR_ID = -3;

Coordinate normal;
Coordinate point;

void fire_cannon(){
    ToneBall *tb = cannon->fire((rand() % 40) + 50, (float)((rand() % 400) + 100));
    if(tb != NULL){
        balls.push_back(tb);
    }
}

void fire_cannon(int midi_note){
    ToneBall *tb = cannon->fire(midi_note, 250);
    if(tb != NULL){
        balls.push_back(tb);
    }
}

void make_new_object(int midi_note){
    if(next_object.mode == TONE_BRICK){
        ToneBrick *nb = (ToneBrick *)next_object.make_object(midi_note);
        if(nb != NULL){
            bricks.push_back(nb);
        }
    }
    else if(next_object.mode == TONE_SPHERE){
        ToneBall *tb = (ToneBall *)next_object.make_object(midi_note);
        if(tb != NULL){
            spheres.push_back(tb);
        }
    }
    else if(next_object.mode == BLACK_HOLE){
        ToneBall *tb = (ToneBall *)next_object.make_object(midi_note);
        if(tb != NULL){
            holes.push_back(tb);
        }
    }
    else if(next_object.mode == BEND_SPHERE){
    }
    else if(next_object.mode == MODULATION_FIELD){
    }
}

void make_new_object(){
    make_new_object((rand() % 40) + 50);
}

bool play_original_notes = false;
bool play_no_notes = false;

void update_frame(float elapsed_time){

    cannon->update(elapsed_time);

    if(cannon->will_fire()){
        has_note_list_changed();
        for(NoteMessage nm : note_messages){
            fire_cannon(nm.midi_note);
        }
        note_messages.clear();
    }
    else if(has_note_list_changed()){
        if(note_messages.size() > 0){
            std::list<NoteMessage>::iterator it = note_messages.begin();
            make_new_object(it->midi_note);
            for(Circle *tb : balls){
                ((ToneBall *)tb)->midi_note = it->midi_note + 12 * (rand() % 3);
                ++it;
                if(it == note_messages.end()){
                    it = note_messages.begin();
                }
            }
            for(Area *tb : bricks){
                ((ToneBrick *)tb)->midi_note = it->midi_note - 12 * (1 + rand() % 2);
                ++it;
                if(it == note_messages.end()){
                    it = note_messages.begin();
                }
            }
            for(Circle *tb : spheres){
                ((ToneBall *)tb)->midi_note = it->midi_note - 12 * (1 + rand() % 2);
                ++it;
                if(it == note_messages.end()){
                    it = note_messages.begin();
                }
            }
            play_original_notes = false;
        }
        else{
            play_original_notes = true;
            for(Area *brick : bricks){
                ((ToneBrick *)brick)->midi_note = ((ToneBrick *)brick)->original_note;
            }
            for(Circle *ball : balls){
                ((ToneBall *)ball)->midi_note = ((ToneBall *)ball)->original_note;
            }
        }
    }

    bool something_erased = false;
    for(std::list<Circle *>::iterator balls_i = balls.begin(); balls_i != balls.end(); ++balls_i){
        ToneBall *circle = (ToneBall*)(*balls_i);
        ((ToneBall*)circle)->update_auto(elapsed_time);

        std::list<Circle *>::iterator balls_j = balls_i;
        ++balls_j;
        for(; balls_j != balls.end(); ++balls_j){
            if(circle->intersects((*balls_j), normal, point)){
                circle->center = point + circle->radius * normal;
                // circle->velocity.bounce_off(normal);
                // ((ToneBall*)(*balls_j))->velocity.bounce_off(normal, false);
                Coordinate hit_power = (circle->velocity.parallel_part(normal) - ((ToneBall*)(*balls_j))->velocity.parallel_part(normal));
                circle->velocity = circle->velocity - hit_power;
                ((ToneBall*)(*balls_j))->velocity = ((ToneBall*)(*balls_j))->velocity + hit_power;
                circle->update_auto(elapsed_time);
                ((ToneBall*)(*balls_j))->update_auto(elapsed_time);
                if(!play_no_notes){
                    living_notes.push_back(new CollisionNote(circle->midi_note, 127, 0.5));
                    living_notes.push_back(new CollisionNote(((ToneBall*)(*balls_j))->midi_note, 127, 0.5));
                }
            }
        }

        for(Area * brick : bricks){
            if(circle->intersects(((ToneBall*)circle)->velocity, brick, normal, point)){
                circle->center = point + circle->radius * normal;
                ((ToneBall*)circle)->velocity.bounce_off(normal);
                ((ToneBall*)circle)->update_auto(elapsed_time);
                if(!play_no_notes){
                    living_notes.push_back(new CollisionNote(((ToneBall*)circle)->midi_note, 127, 0.5));
                    living_notes.push_back(new CollisionNote(((ToneBrick*)brick)->midi_note, 127, 0.5));
                }
            }
        }
        for(Circle * sphere : spheres){
            if(circle->intersects(sphere, normal, point)){
                circle->center = point + circle->radius * normal;
                // ((ToneBall*)circle)->velocity.bounce_off(normal);
                Coordinate hit_power = (circle->velocity.parallel_part(normal) - ((ToneBall*)sphere)->velocity.parallel_part(normal));
                circle->velocity = circle->velocity - 2 * hit_power;
                float l = circle->velocity.length();
                if(l > 1600.0){
                    circle->velocity = 1600.0 * circle->velocity / l;
                }
                ((ToneBall*)circle)->update_auto(elapsed_time);
                if(!play_no_notes){
                    living_notes.push_back(new CollisionNote(((ToneBall*)circle)->midi_note, 127, 0.5));
                    living_notes.push_back(new CollisionNote(((ToneBall*)sphere)->midi_note, 127, 0.5));
                }
            }
        }
        for(Circle * hole : holes){
            if(circle->intersects(hole, normal, point)){
                delete *balls_i;
                balls.erase(balls_i);
                something_erased = true;
                break;
                if(!play_no_notes){
                    // living_notes.push_back(new CollisionNote(((ToneBall*)circle)->midi_note, 127, 0.5));
                }
            }
        }
        if(something_erased){
            continue;
        }
        for(Area * area : outside_edges){
            if(circle->intersects(((ToneBall*)circle)->velocity, area, normal, point)){
                circle->center = point + circle->radius * normal;
                ((ToneBall*)circle)->velocity.bounce_off(normal);
                ((ToneBall*)circle)->update_auto(elapsed_time);
                if(!play_no_notes){
                    living_notes.push_back(new CollisionNote(((ToneBall*)circle)->midi_note - 36, 127, 2.5));
                }
            }
        }

        if(circle->center.x < 0){
            circle->center.x = EDGE_WIDTH + circle->radius + 10;
            ((ToneBall*)circle)->velocity.x = abs(((ToneBall*)circle)->velocity.x);
        }
        else if(circle->center.x > screen_width){
            circle->center.x = screen_width - EDGE_WIDTH - circle->radius - 10;
            ((ToneBall*)circle)->velocity.x = -abs(((ToneBall*)circle)->velocity.x);
        }
        if(circle->center.y < 0){
            circle->center.y = circle->radius + 10;
            ((ToneBall*)circle)->velocity.y = abs(((ToneBall*)circle)->velocity.y);
        }
        else if(circle->center.y > screen_height){
            circle->center.y = screen_height - circle->radius - 10;
            ((ToneBall*)circle)->velocity.y = -abs(((ToneBall*)circle)->velocity.y);
        }
    }

    for(std::list<CollisionNote *>::iterator it = living_notes.begin(); it != living_notes.end(); ++it){
        if(!(*it)->update(elapsed_time)){
            delete *it;
            living_notes.erase(it);
        }
    }
}

void clean_up(){
    
    for(CollisionNote *note : living_notes){
        send_midi_note(note->midi_note, 0);
    }
    Sleep(500);
}

void onDisplay(void) {
    int d;
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    draw_areas(bricks);
    next_object.draw();

    for(Circle * circle : holes){
        ((ToneBall*)circle)->draw(true);
    }

    for(Circle * circle : spheres){
        ((ToneBall*)circle)->draw(false);
    }

    for(Circle * circle : balls){
        ((ToneBall*)circle)->draw();
    }

    cannon->draw();

    draw_areas(outside_edges);

    Area *a = object_mode_buttons[next_object.mode];
    draw_rectangle(a->center.x, a->center.y, a->width + 20, a->height + 20, Color(1, 1, 1));
    a = object_mode_buttons[next_object.mode + 3];
    draw_rectangle(a->center.x, a->center.y, a->width + 20, a->height + 20, Color(1, 1, 1));
    draw_areas(object_mode_buttons);

    for(int i = 0; i < object_mode_buttons.size(); i += 3){
        a = object_mode_buttons[i];
        draw_rectangle(a->center.x, a->center.y, a->width - 40, a->height - 40, Color(1, 0, 0));
        a = object_mode_buttons[i+1];
        draw_circle(a->center.x, a->center.y, a->width / 2 - 20, Color(0, 0, 1), 24, false);
        a = object_mode_buttons[i+2];
        draw_circle(a->center.x, a->center.y, a->width / 2 - 20, Color(0.6, 0.6, 0.6), 24);
    }

    glutSwapBuffers();
}

/* Using FG2.8 (reversed) prototype for now */
/* void onMultiButton(int cursor_id, int button, int state, int x, int y) { */
void onMultiButton(int cursor_id, int x, int y, int button, int state) {
    bool touch_not_found = true;
    // printf("MULTIBUTTON, cursor: %d, x: %d, y: %d, button: %d, state: %d\n", cursor_id, x, y, button, state);
    if(state == GLUT_DOWN){
        if(cannon->touch(cursor_id, x, screen_height - y)){
            touch_not_found = false;
        }
        if(touch_not_found){
            for(int i = 0; i < 3; ++i){
                if(object_mode_buttons[i]->is_inside(x, screen_height - y) || object_mode_buttons[i + 3]->is_inside(x, screen_height - y)){
                    next_object.set_mode(ObjectMode(i));
                    touch_not_found = false;
                    break;
                }
            }
        }
        if(touch_not_found){
            for(Area *brick : bricks){
                if(((ToneBrick *)brick)->touch(cursor_id, x, screen_height - y)){
                    touch_not_found = false;
                    break;
                }
            }
        }
        if(touch_not_found){
            for(Circle *sphere : spheres){
                if(((ToneBall *)sphere)->touch(cursor_id, x, screen_height - y)){
                    touch_not_found = false;
                    break;
                }
            }
        }
        if(touch_not_found){
            for(Circle *hole : holes){
                if(((ToneBall *)hole)->touch(cursor_id, x, screen_height - y)){
                    touch_not_found = false;
                    break;
                }
            }
        }
        if(touch_not_found){
        }
        if(touch_not_found && next_object.touch(cursor_id, x, screen_height - y)){
            touch_not_found = false;
        }
    }
    else{
        if(cannon->untouch(cursor_id)){
            touch_not_found = false;
        }
        if(touch_not_found && next_object.untouch(cursor_id)){
            touch_not_found = false;
        }
        if(touch_not_found){
            for(std::list<Area *>::iterator brick = bricks.begin(); brick != bricks.end(); ++brick){
                if(((ToneBrick *)(*brick))->untouch(cursor_id, EDGE_WIDTH, screen_width - 2 * EDGE_WIDTH)){
                    touch_not_found = false;
                    if(((ToneBrick *)(*brick))->invalid){
                        delete *brick;
                        bricks.erase(brick);
                    }
                    break;
                }
            }
        }
        if(touch_not_found){
            for(std::list<Circle *>::iterator sphere = spheres.begin(); sphere != spheres.end(); ++sphere){
                if(((ToneBall *)(*sphere))->untouch(cursor_id, EDGE_WIDTH, screen_width - 2 * EDGE_WIDTH)){
                    touch_not_found = false;
                    if(((ToneBall *)(*sphere))->invalid){
                        delete *sphere;
                        spheres.erase(sphere);
                    }
                    break;
                }
            }
        }
        if(touch_not_found){
            for(std::list<Circle *>::iterator hole = holes.begin(); hole != holes.end(); ++hole){
                if(((ToneBall *)(*hole))->untouch(cursor_id, EDGE_WIDTH, screen_width - 2 * EDGE_WIDTH)){
                    touch_not_found = false;
                    if(((ToneBall *)(*hole))->invalid){
                        delete *hole;
                        holes.erase(hole);
                    }
                    break;
                }
            }
        }
        if(touch_not_found){
        }
    }
}

bool random_has_not_been_seeded = true;
int random_seed = 0;

void onMultiMotion(int cursor_id, int x, int y) {
    if(random_has_not_been_seeded){
        random_seed = (random_seed + x * 7 + y * 11) % 10000000;
    }
    bool touch_not_found = true;
    if(cannon->touch_motion(cursor_id, x, screen_height - y)){
        touch_not_found = false;
    }
    if(touch_not_found && next_object.touch_motion(cursor_id, x, screen_height - y)){
        touch_not_found = false;
    }
    if(touch_not_found){
        for(Area *brick : bricks){
            if(((ToneBrick *)brick)->touch_motion(cursor_id, x, screen_height - y)){
                touch_not_found = false;
                break;
            }
        }
    }
    if(touch_not_found){
        for(Circle *sphere : spheres){
            if(((ToneBall *)sphere)->touch_motion(cursor_id, x, screen_height - y)){
                touch_not_found = false;
                break;
            }
        }
    }
    if(touch_not_found){
        for(Circle *hole : holes){
            if(((ToneBall *)hole)->touch_motion(cursor_id, x, screen_height - y)){
                touch_not_found = false;
                break;
            }
        }
    }
    if(touch_not_found){
    }
    // printf("MULTIMOTION, cursor: %d, x: %d, y: %d\n", cursor_id, x, y);
}

void onKeyDown(unsigned char key, int x, int y){
    if(key == ' '){
        if(random_has_not_been_seeded){
            random_has_not_been_seeded = false;
            srand(random_seed);
        }
        fire_cannon();
        make_new_object();
    }
    else if(key == '5'){
        next_object.set_mode(TONE_BRICK);
    }
    else if(key == '8'){
        next_object.set_mode(TONE_SPHERE);
    }
    else if(key == '2'){
        next_object.set_mode(BLACK_HOLE);
    }
    else if(key == 'q' || key == 'Q'){
        clean_up();
        glutExit();
    }
}

void onKeyUp(unsigned char key, int x, int y){
    
}

void onReshape(int width, int height) {
    screen_width = width;
    screen_height = height;

    cannon->reset_sizes((float)width / 2.0, (float)width / 40.0, (float)width / 4.0, (float)width / 20.0);

    for(int i = 0; i < object_mode_buttons.size(); i += 3){
        object_mode_buttons[i]->center.y = screen_height / 2 + 120;
        object_mode_buttons[i + 1]->center.y = screen_height / 2;
        object_mode_buttons[i + 2]->center.y = screen_height / 2 - 120;
    }
    for(int i = 3; i < object_mode_buttons.size(); ++i){
        object_mode_buttons[i]->center.x = screen_width - EDGE_WIDTH / 2;
    }

    outside_edges[0]->center.y = screen_height / 2;
    outside_edges[0]->height = screen_height;
    outside_edges[1]->center.x = screen_width - EDGE_WIDTH / 2.0;  // RIGHT SIDE
    outside_edges[1]->center.y = screen_height / 2;
    outside_edges[1]->height = screen_height;
    outside_edges[2]->center.x = screen_width / 2;
    outside_edges[2]->center.y = screen_height + 100;
    outside_edges[2]->width = screen_width;
    outside_edges[3]->center.x = screen_width / 2;
    outside_edges[3]->width = screen_width;
    outside_edges[4]->center.y = screen_height / 2;
    outside_edges[4]->height = screen_height;
    outside_edges[5]->center.x = screen_width - EDGE_WIDTH;  // RIGHT SIDE
    outside_edges[5]->center.y = screen_height / 2;
    outside_edges[5]->height = screen_height;

    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);


    //printf("RESHAPE !!!\n");
}

int previous_time = 0;

void onIdle(void) {
    if(previous_time == 0){
        previous_time = glutGet(GLUT_ELAPSED_TIME);
    }
    Sleep(10);
    int new_time = glutGet(GLUT_ELAPSED_TIME);
    float delta_time = float(new_time - previous_time) / 1000.0;
    previous_time = new_time;
    update_frame(delta_time);
    glutPostRedisplay();
}

int init_opengl_glut(int &argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutInitWindowSize(screen_width, screen_height);
    glutInitWindowPosition(960 - screen_width / 2, 512 - screen_height / 2);
    glutCreateWindow("Knocktunal Bendfest");

    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutIdleFunc(onIdle);

    glutIgnoreKeyRepeat(true);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);

    glutMultiButtonFunc(onMultiButton);
    glutMultiMotionFunc(onMultiMotion);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutFullScreen();

    glutMainLoop();

    clean_up();

    return EXIT_SUCCESS;
}
