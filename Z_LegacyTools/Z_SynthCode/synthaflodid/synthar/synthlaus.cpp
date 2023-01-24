/** @file synthlaus.cpp
    @ingroup synthaflodid
    @brief Send midi data based on multitouch input.
    @author Kári Halldórsson
*/

#include <iostream>
#include <stdio.h>
#include <math.h>

#include "helpers_graphics.h"
#include "helpers_midi.h"

#include <vector>
#include <unordered_map>


struct Key : public Area{
    Key(float x, float y, float w, float h, float c, int note) : Area(x, y, w, h, c){
        midi_note = note;
    };
    int midi_note;

    // bool is_inside(float x, float y){
    //     return area.is_inside(x, y);
    // }

    int get_note(){
        return midi_note;
    }
};


std::vector<Area *> keyboard;
Area octave_down_button(150, 350, 280, 80, 0.5);
Area octave_up_button(1450, 350, 280, 80, 0.5);

int note_offset = 0;

std::unordered_map<int, int> current_notes;


int init_opengl_glut(int &argc, char* argv[]);

int main(int argc, char* argv[])
{
    init_midi_out();

    float key_pos = 40;
    int note = 48;
    for(int i = 0; i < 22; i++){
        keyboard.push_back(new Key(key_pos, 120, 68, 198, 1.0, note));
        if(i % 7 == 2 || i % 7 == 6){
            note -= 1;
        }
        note += 2;
        key_pos += 70;
    }
    key_pos = 65;
    note = 49;
    for(int i = 0; i < 15; i++){
        keyboard.push_back(new Key(key_pos, 220, 58, 148, 0.3, note));
        if(i % 5 == 1 || i % 5 == 4){
            note += 1;
            key_pos += 70;
        }
        note += 2;
        key_pos += 70;
    }

    init_opengl_glut(argc, argv);

    uninit_midi_out();
    printf("Test finished.\n");
}

bool check_keyboard(int x, int y, int &current_note){
    for(int i = keyboard.size() - 1; i >= 0; i--){
        Key *key = (Key*)keyboard[i];
        if(key->is_inside(x, y)){
            //printf("The note: %d\n", key->get_note());
            int note = key->get_note() + note_offset;
            if(current_note != note){
                // NOTE ON
                send_midi_note(note);
                if(current_note >= 0){
                    // NOTE OFF (NOTE ON (0))
                    send_midi_note(current_note, 0);
                }
                current_note = note;
            }
            return true;
        }
    }
    if(octave_down_button.is_inside(x, y)){
        if(current_note != -2){
            note_offset -= 12;
            if(note_offset < -48){
                note_offset+= 12;
            }
            current_note = -2;
        }
        return true;
    }
    if(octave_up_button.is_inside(x, y)){
        if(current_note != -3){
            note_offset += 12;
            if(note_offset > 36){
                note_offset -= 12;
            }
            current_note = -3;
        }
        return true;
    }
    return false;
}

void check_note_off(int &current_note){
    if(current_note >= 0){
        // NOTE OFF (NOTE ON (0))
        send_midi_note(current_note, 0);
    }
    current_note = -1;
}








#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/freeglut.h>

int screen_width = 1600;
int screen_height = 400;

bool normal_mouse_down = false;
int touch_active = 2;
const static int NORMAL_MOUSE_CURSOR_ID = -3;

void onDisplay(void) {
    int d;
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // for(int i = 0; i < keyboard.size(); i++){
    //     // Key *key = (Key*)keyboard[i];
    //     // draw_area(*key);
    //     draw_area(*keyboard[i]);
    // }
    draw_areas(keyboard);

    draw_area(octave_down_button);
    draw_area(octave_up_button);

    glutSwapBuffers();
}

void onMultiEntry(int cursor_id, int state) {
    touch_active = 2;
    //printf("MULTIEntry, cursor: %d, state: %d\n", cursor_id, state);

    if(state == GLUT_ENTERED){
        if(normal_mouse_down){
            normal_mouse_down = false;
            check_note_off(current_notes[NORMAL_MOUSE_CURSOR_ID]);
            current_notes.erase(NORMAL_MOUSE_CURSOR_ID);
            printf("NORMAL MOUSE");
        }
        current_notes[cursor_id] = -1;
    }
    else{
        current_notes.erase(cursor_id);
    }
}

/* Using FG2.8 (reversed) prototype for now */
/* void onMultiButton(int cursor_id, int button, int state, int x, int y) { */
void onMultiButton(int cursor_id, int x, int y, int button, int state) {
    touch_active = 2;
    //printf("MULTIBUTTON, cursor: %d, x: %d, y: %d, button: %d, state: %d\n", cursor_id, x, y, button, state);
    if(state == GLUT_DOWN){
        check_keyboard(x, screen_height - y, current_notes[cursor_id]);
        //printf("MOUSE DOWN\n");
    }
    else{
        check_note_off(current_notes[cursor_id]);
        //printf("MOUSE UP\n");
    }
}

void onMultiMotion(int cursor_id, int x, int y) {
    touch_active = 2;
    //printf("MULTIMOTION, cursor: %d, x: %d, y: %d\n", cursor_id, x, y);
    if(!check_keyboard(x, screen_height - y, current_notes[cursor_id])){
        check_note_off(current_notes[cursor_id]);
    }
}

void onMultiPassive(int cursor_id, int x, int y) {
    //printf("MULTIPassive, cursor: %d, x: %d, y: %d\n", cursor_id, x, y);
}

void onEntry(int state){
    if(state == GLUT_ENTERED){
        printf("MOUSE ENTERED\n");
    }
    else{
        printf("MOUSE LEFT\n");
    }
}

void onMouse(int button, int state, int x, int y) {
    // if (button == 0) {
    //     cursors[0][0].on = (state == GLUT_DOWN);
    //     cursors[0][0].x = (float)x;
    //     cursors[0][0].y = (float)y;
    //     //printf("normal click\n");
        
    //     old_x = x;
    //     old_y = y;
    if(button == 0){
        if(state == GLUT_DOWN){
            if(touch_active > 0){
                return;
            }
            if(current_notes.size() == 0){
                onMultiEntry(NORMAL_MOUSE_CURSOR_ID, GLUT_ENTERED);
                normal_mouse_down = true;
                onMultiButton(NORMAL_MOUSE_CURSOR_ID, x, y, button, state);
            }
            //printf("MOUSE DOWN\n");
        }
        else{
            if(normal_mouse_down){
                normal_mouse_down = false;
                onMultiButton(NORMAL_MOUSE_CURSOR_ID, x, y, button, state);
                onMultiEntry(NORMAL_MOUSE_CURSOR_ID, GLUT_LEFT);
                touch_active = 0;
            }
            //printf("MOUSE UP\n");
        }
    }
}

void onMotion(int x, int y) {
    //printf("MOTION, x: %d, y: %d\n", x, y);
    if(normal_mouse_down){
        onMultiMotion(NORMAL_MOUSE_CURSOR_ID, x, y);
    }
}

void onPassive(int x, int y) {
    if(touch_active > 0) {touch_active -= 1;}
    //printf("PASSIVE, x: %d, y: %d, touch_active: %d\n", x, y, touch_active);
}

void onReshape(int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);


    //printf("RESHAPE !!!\n");
}

void onIdle(void) {
    Sleep(10);
    glutPostRedisplay();
}

int init_opengl_glut(int &argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutInitWindowSize(screen_width, screen_height);
    glutInitWindowPosition(0, 420);
    glutCreateWindow("Sa ydar sem synthlaus er");

    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutIdleFunc(onIdle);

    //glutEntryFunc(onEntry);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutPassiveMotionFunc(onPassive);

    glutMultiEntryFunc(onMultiEntry);
    glutMultiButtonFunc(onMultiButton);
    glutMultiMotionFunc(onMultiMotion);
    //glutMultiPassiveFunc(onMultiPassive);


    glutMainLoop();

    return EXIT_SUCCESS;
}
