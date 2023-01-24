/** @file daudasynthinn.cpp
    @ingroup synthaflodid
    @brief Play music with FM synthesis.
    @author Kári Halldórsson
*/

//#include <iostream>
//#include <stdio.h>
//#include <math.h>
//#include <stdlib.h>
//#include <string.h>

#include <vector>
#include <map>

#include <GL/freeglut.h>

#include "helpers_midi.h"
#include "helpers_graphics.h"
#include "daudasynthinn_music.h"
#include "daudasynthinn_ui.h"
#include "notes.h"

#define MIN_TIME_PER_GRAPHICS_FRAME 20

// PHYSICAL CONTROLS

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

#include <unordered_map>

std::vector<Area *> keyboard;
std::unordered_map<int, int> current_notes;

void init_keyboard(){

    float key_pos = 40;
    int note = 48;
    for(int i = 0; i < 17; i++){
        keyboard.push_back(new Key(key_pos, 60, 68, 118, 1.0, note));
        if(i % 7 == 2 || i % 7 == 6){
            note -= 1;
        }
        note += 2;
        key_pos += 70;
    }
    key_pos = 65;
    note = 49;
    for(int i = 0; i < 12; i++){
        keyboard.push_back(new Key(key_pos, 90, 58, 58, 0.3, note));
        if(i % 5 == 1 || i % 5 == 4){
            note += 1;
            key_pos += 70;
        }
        note += 2;
        key_pos += 70;
    }
}

bool check_keyboard(int x, int y, int &current_note){
    for(int i = keyboard.size() - 1; i >= 0; i--){
        Key *key = (Key*)keyboard[i];
        if(key->is_inside(x, y)){
            //printf("The note: %d\n", key->get_note());
            int note = key->get_note();
            if(current_note != note){
                // NOTE ON
                NoteCollections::get_instance().MIDI_change_note(note, 100);
                if(current_note >= 0){
                    // NOTE OFF (NOTE ON (0))
                    NoteCollections::get_instance().MIDI_change_note(current_note, 0);
                }
                current_note = note;
            }
            return true;
        }
    }
    return false;
}

void check_key_off(int &current_note){
    if(current_note >= 0){
        // NOTE OFF (NOTE ON (0))
        NoteCollections::get_instance().MIDI_change_note(current_note, 0);
    }
    current_note = -1;
}


//TOUCH UI

TouchScreenUI ui;
int previous_time_millis;

int touch_active = 2;
bool normal_mouse_down = false;
const static int NORMAL_MOUSE_CURSOR_ID = -3;

void onIdle(void) {
    int current_time_ms = glutGet(GLUT_ELAPSED_TIME);

    while(current_time_ms - previous_time_millis < MIN_TIME_PER_GRAPHICS_FRAME){
        std::this_thread::sleep_for(std::chrono::milliseconds(MIN_TIME_PER_GRAPHICS_FRAME - (current_time_ms - previous_time_millis)));
        current_time_ms = glutGet(GLUT_ELAPSED_TIME);
    }
    // printf("MAIN TIME: %d\n", current_time_ms - previous_time_millis);

    NoteCollections::get_instance().update_notes(current_time_ms);

    ui.update_ui((float)current_time_ms);

    previous_time_millis = current_time_ms;

    glutPostRedisplay();
}

void onDisplay(void) {

    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

// PHYSICAL CONTROLS

    draw_areas(keyboard);



// TOUCH SCREEN

    draw_rectangle(touch_start_x + 400, touch_start_y + 240, 800, 480, Color(0.0, 0.0, 0.0));

    glPushMatrix();
    glTranslatef(touch_start_x, touch_start_y, 0.0);
    ui.draw_ui();
    glPopMatrix();

    glutSwapBuffers();
}

void onMultiEntry(int cursor_id, int state) {
    touch_active = 2;
    //printf("MULTIEntry, cursor: %d, state: %d\n", cursor_id, state);

    // PHYSICAL CONTROLS
    if(state == GLUT_ENTERED){
        current_notes[cursor_id] = -1;
    }
    else{
        current_notes.erase(cursor_id);
    }

    // TOUCH UI
    ui.cursor_entry(cursor_id, state == GLUT_ENTERED ? false : true);
}

/* Using FG2.8 (reversed) prototype for now */
/* void onMultiButton(int cursor_id, int button, int state, int x, int y) { */
void onMultiButton(int cursor_id, int x, int y, int button, int state) {
    touch_active = 2;

    if (button == 0) {
        ui.cursor_activate(cursor_id, x - touch_start_x, (screen_height - y) - touch_start_y, state == GLUT_DOWN ? false : true);
        if(state == GLUT_UP){
            check_key_off(current_notes[cursor_id]);
            if(normal_mouse_down){
                normal_mouse_down = false;
                // printf("NORMAL MOUSE\n");
            }
        }
        else{
            check_keyboard(x, screen_height - y, current_notes[cursor_id]);
        }
    }
}



void onMultiMotion(int cursor_id, int x, int y) {
    touch_active = 2;

    if(!check_keyboard(x, screen_height - y, current_notes[cursor_id])){
        check_key_off(current_notes[cursor_id]);
    }

    ui.check_cursors(cursor_id, x - touch_start_x, (screen_height - y) - touch_start_y);
}

void onPassive(int x, int y) {
    if(touch_active > 0) {touch_active -= 1;}
}

void onMouse(int button, int state, int x, int y) {

    if(button == 0){
        if(state == GLUT_DOWN){
            if(touch_active > 0){
                return;
            }
            //if(cursors.size() == 0){
                onMultiEntry(NORMAL_MOUSE_CURSOR_ID, GLUT_ENTERED);
                normal_mouse_down = true;
                onMultiButton(NORMAL_MOUSE_CURSOR_ID, x, y, button, state);
                // printf("NORMAL MOUSE DOWN\n");
                onMultiMotion(NORMAL_MOUSE_CURSOR_ID, x, y);
            //}
        }
        else{
            if(normal_mouse_down){
                normal_mouse_down = false;
                onMultiButton(NORMAL_MOUSE_CURSOR_ID, x, y, button, state);
                onMultiEntry(NORMAL_MOUSE_CURSOR_ID, GLUT_LEFT);
                touch_active = 0;
                // printf("NORMAL MOUSE UP\n");
            }
            
        }
    }
}

void onMotion(int x, int y) {

    if(normal_mouse_down){
        onMultiMotion(NORMAL_MOUSE_CURSOR_ID, x, y);
    }
}

void onReshape(int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
}

int init_opengl_glut(int &argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutInitWindowSize(screen_width, screen_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Daudasynthinn");

    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutIdleFunc(onIdle);

    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutPassiveMotionFunc(onPassive);

    glutMultiEntryFunc(onMultiEntry);
    glutMultiButtonFunc(onMultiButton);
    glutMultiMotionFunc(onMultiMotion);

    glutMainLoop();

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    init_midi_in(midiCallback);
    printf("3\n");

    if(!init_portaudio()){
        return 0;
    }

    init_keyboard();

    init_opengl_glut(argc, argv);

    if(!uninit_portaudio()){
        return 0;
    }

    uninit_midi_in();
    printf("Test finished.\n");
}


